#include "VirtualMemoryManager.hpp"

#include <algorithm>

#undef min

void VirtualMemoryManager::analyseFile(const std::string & fileName)
{
	//analyse the volume data
	//we need compute the size of volume and other information
	//to do:

	mFile.sync_with_stdio(false);
	//mFile.open(fileName, std::ios::binary);
	mFileSize = Size(1000, 1000, 1000);
}

void VirtualMemoryManager::mapAddressToGPU(int resolution, const glm::vec3 & position, BlockCache * block)
{
	//upload block data to GPU virtual memory
	mGPUDirectoryCache->mapAddress(resolution, position, block);
}

void VirtualMemoryManager::initialize(const std::string &fileName, const std::vector<glm::vec3> &resolution)
{
	//initialize the virutal memroy resource(CPU and GPU)

	//expand size of CPU virtual memory
	//if the size of CPU and GPU are same, it can not work well
	const int expand = 2;

	analyseFile(fileName);
	
	//set cache size
	PageCache::setPageCacheSize(PAGE_SIZE_XYZ);
	BlockCache::setBlockCacheSize(BLOCK_SIZE_XYZ);

	//for CPU virtual memory we expand the size of block and page table
	mBlockCacheTable = new BlockTable(BLOCK_COUNT_XYZ + expand);
	mPageCacheTable = new PageTable(PAGE_COUNT_XYZ + expand, mBlockCacheTable);
	
	assert(resolution.size() == ((size_t)MULTIRESOLUTION_COUNT));

	//initialize resolution 0 directory
	//page size means the size of one page cache can store
	int pageSize = PAGE_SIZE_XYZ * BLOCK_SIZE_XYZ;

	//the count of block for multi-resolution
	std::vector<int> multiResolutionBlock;

	//for all resolution, we compute the real size
	//then we compute the directory size we need
	for (size_t i = 0; i < resolution.size(); i++) {
		auto realSize = Helper::multiple(mFileSize, resolution[i]);
		auto directorySize = Size(
			(int)ceil((float)realSize.X / pageSize),
			(int)ceil((float)realSize.Y / pageSize),
			(int)ceil((float)realSize.Z / pageSize)
		);
		
		//block count size
		auto blockSize = Size(
			(int)ceil((float)realSize.X / BLOCK_SIZE_XYZ),
			(int)ceil((float)realSize.Y / BLOCK_SIZE_XYZ),
			(int)ceil((float)realSize.Z / BLOCK_SIZE_XYZ)
		);

		mMultiResolutionSize.push_back(directorySize);
		multiResolutionBlock.push_back(blockSize.X * blockSize.Y * blockSize.Z);
	}

	mMultiResolutionBase.push_back(0);
	mMultiResolutionBlockBase.push_back(0);

	for (size_t i = 1; i < resolution.size(); i++) {
		auto base = VirtualAddress(mMultiResolutionBase[i - 1].X + mMultiResolutionSize[i - 1].X, 0, 0);
		auto blockBase = mMultiResolutionBlockBase[i - 1] + multiResolutionBlock[i - 1];

		mMultiResolutionBase.push_back(base);
		mMultiResolutionBlockBase.push_back(blockBase);
		mMultiResolutionBlockEnd.push_back(blockBase - 1);
	}

	//compute the end of block count for multi-resolution
	mMultiResolutionBlockEnd.push_back(
		mMultiResolutionBlockBase[mMultiResolutionBlockBase.size() - 1] +
		multiResolutionBlock[multiResolutionBlock.size() - 1] - 1);

	//init page directory cache with multi-resolution
	mDirectoryCache = new PageDirectory(mMultiResolutionSize, mPageCacheTable);

	//create buffer for resolution
	mMultiResolutionSizeBuffer = mFactory->createConstantBuffer(sizeof(Size) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionBaseBuffer = mFactory->createConstantBuffer(sizeof(VirtualAddress) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionBlockBaseBuffer = mFactory->createConstantBuffer(sizeof(int) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionSizeBuffer->update(&mMultiResolutionSize[0]);
	mMultiResolutionBaseBuffer->update(&mMultiResolutionBase[0]);
	mMultiResolutionBlockBaseBuffer->update(&mMultiResolutionBlockBase[0]);

	//init the GPU resource(Texture3D and ResourceUsage)
	mGPUBlockCacheTable = new GPUBlockTable(mFactory, mGraphics, BLOCK_COUNT_XYZ);
	mGPUPageCacheTable = new GPUPageTable(mFactory, mGraphics, PAGE_COUNT_XYZ, mGPUBlockCacheTable);
	mGPUDirectoryCache = new GPUPageDirectory(mFactory, mGraphics, mMultiResolutionSize, mGPUPageCacheTable);

	//for current version, we use one byte to store a block state(it is simple, may be changed in next version) 
	//and we do not use hash to avoid the same block problem(will be solved in next version)
	//and we report one unsigned int as block id and resolution level
	//for block cache miss texture, we need compute the x and y size and make sure it can cover the render target
	//so the x and y size is equal "ceil(mResolutionSize / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY)"
	//and the z size means the array size(0 for count)
	int blockCacheUsageStateSize = BLOCK_COUNT_XYZ;
	int blockCacheMissArraySizeX = (int)ceil((float)mResolutionWidth / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY);
	int blockCacheMissArraySizeY = (int)ceil((float)mResolutionHeight / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY);
	int blockCacheMissArraySizeZ = BLOCK_HASH_TABLE_ARRAY_SIZE;

	//create texture 3d and resource usage
	mBlockCacheUsageStateTexture = new SharedTexture3D(
		blockCacheUsageStateSize, 
		blockCacheUsageStateSize, 
		blockCacheUsageStateSize, 
		PixelFormat::R8Uint,
		ResourceInfo::UnorderedAccess(),
		mFactory);
	
	mBlockCacheMissArrayTexture = new SharedTexture3D(
		blockCacheMissArraySizeX,
		blockCacheMissArraySizeY,
		blockCacheMissArraySizeZ,
		PixelFormat::R32Uint,
		ResourceInfo::UnorderedAccess(),
		mFactory);

	mBlockCacheUsageStateUsage = mFactory->createUnorderedAccessUsage(
		mBlockCacheUsageStateTexture->getGpuTexture(), 
		mBlockCacheUsageStateTexture->getPixelFormat());

	mBlockCacheMissArrayUsage = mFactory->createUnorderedAccessUsage(
		mBlockCacheMissArrayTexture->getGpuTexture(), 
		mBlockCacheMissArrayTexture->getPixelFormat());
}

void VirtualMemoryManager::solveCacheMiss()
{
	//update the GpuTexture to CpuTexture
	mBlockCacheUsageStateTexture->update();
	mBlockCacheMissArrayTexture->update();

	//solve the usage state texture
	//map the texture to memory
	auto usageState = mBlockCacheUsageStateTexture->mapCpuTexture();

	//get data pointer
	byte* usageStateData = (byte*)usageState.Data;
	//get array size, because of padding the size is not equal the texture size
	auto endPosition = usageState.DepthPitch * mBlockCacheUsageStateTexture->getDepth();

	//check state
	for (int id = 0; id < endPosition; id++) {

		if (usageStateData[id] == 0) continue;

		//get address
		//id = x + y * rowPitch + z * depthPitch
		//because of usageState is byte
		auto address = VirtualAddress(
			(id % usageState.DepthPitch) % usageState.RowPitch,
			(id % usageState.DepthPitch) / usageState.RowPitch,
			(id / usageState.DepthPitch)
		);

		//update the LRU system, we need update the directory, pages and block
		//so it is not right in this version
		mGPUBlockCacheTable->getAddress(address);
	}

	//unmap
	mBlockCacheUsageStateTexture->unmapCpuTexture();

	//solve the cache miss
	//map the texture to memory
	auto cacheMiss = mBlockCacheMissArrayTexture->mapCpuTexture();

	//get data pointer
	unsigned int* cacheMissData = (unsigned int*)cacheMiss.Data;

	//reset the row pitch and depth pitch to size
	//the cache miss is unsigned int
	cacheMiss.RowPitch = cacheMiss.RowPitch / sizeof(unsigned int);
	cacheMiss.DepthPitch = cacheMiss.DepthPitch / sizeof(unsigned int);

	auto xEndPosition = mBlockCacheMissArrayTexture->getWidth();
	auto yEndPosition = mBlockCacheMissArrayTexture->getHeight();
	
	for (size_t x = 0; x < xEndPosition; x++) {
		for (size_t y = 0; y < yEndPosition; y++) {
			//we store count at (x, y, 0) -> x + y * rowPitch + 0 * depthPitch
			auto cacheMissCount = cacheMissData[x + y * cacheMiss.RowPitch];

			for (size_t z = 1; z <= cacheMissCount; z++) {
				auto id = cacheMissData[x + y * cacheMiss.RowPitch + z * cacheMiss.DepthPitch];

				//get resolution 
				auto resolution = unsigned int(
					std::lower_bound(mMultiResolutionBlockEnd.begin(), mMultiResolutionBlockEnd.end(), id) - mMultiResolutionBlockEnd.begin()) 
					/ (unsigned int)sizeof(unsigned int);
	
				id = id - mMultiResolutionBlockBase[resolution];

				mapAddress(resolution, id);
			}
		}
	}

	mBlockCacheMissArrayTexture->unmapCpuTexture();
}

void VirtualMemoryManager::finalize() 
{
	delete mDirectoryCache;
	delete mPageCacheTable;
	delete mBlockCacheTable;

	delete mGPUDirectoryCache;
	delete mGPUPageCacheTable;
	delete mGPUBlockCacheTable;

	delete mBlockCacheUsageStateTexture;
	delete mBlockCacheMissArrayTexture;

	mFactory->destoryUnorderedAccessUsage(mBlockCacheUsageStateUsage);
	mFactory->destoryUnorderedAccessUsage(mBlockCacheMissArrayUsage);
}

void VirtualMemoryManager::mapAddress(int resolution, int blockID) 
{
	//for each cache miss, we will test if the block in the CPU virtual memory
	//if it is not in the memory, we will load it from disk
	//if it is in the memory, we will upload it to GPU virtual memory

	assert(resolution < MULTIRESOLUTION_COUNT);

	//get the directory cache size of current resolution
	auto directoryCacheSize = mDirectoryCache->getResolutionSize(resolution);
	auto blockCacheSize = Helper::multiple(directoryCacheSize, PageCache::getPageCacheSize());

	//if block is legal
	assert(blockID >= 0 && blockID < blockCacheSize.X * blockCacheSize.Y * blockCacheSize.Z);

	//row pitch is equal width and depth pitch is equal (width * height)
	auto rowPitch = blockCacheSize.X;
	auto depthPitch = blockCacheSize.X * blockCacheSize.Y;

	//block id is equal z * (depth pitch) + y * (row pitch) + x
	//so z = (block id) / (depth pitch)
	//so y = ((block id) % (depth pitch)) / (row pitch)
	//so x = (block id % (depth pitch)) % (row pitch)
	auto blockAddress = VirtualAddress(
		(blockID % depthPitch) % rowPitch,
		(blockID % depthPitch) / rowPitch,
		(blockID / depthPitch)
	);

	//get center position for the block test
	auto blockCenterPosition = glm::vec3(
		(blockAddress.X + 0.5f) / blockCacheSize.X,
		(blockAddress.Y + 0.5f) / blockCacheSize.Y,
		(blockAddress.Z + 0.5f) / blockCacheSize.Z
	);

	//query the block if in the CPU virtual memory
	BlockCache* blockCache = mDirectoryCache->queryAddress(resolution, blockCenterPosition);

	//not, we load from disk and add it in to CPU virtual memory
	if (blockCache == nullptr) {
		//load block data from disk and do some special process
		static BlockCache output(BLOCK_SIZE_XYZ);

		loadBlock(resolution, blockAddress, output);
		
		blockCache = &output;

		mDirectoryCache->mapAddress(resolution, blockCenterPosition, blockCache);
	}

	//now, we need upload the block to GPU virtual memory
	mapAddressToGPU(resolution, blockCenterPosition, blockCache);
}

void VirtualMemoryManager::loadBlock(int resolution, const VirtualAddress & blockAddress, BlockCache & output) 
{
	//load block data
	//note: need discuss the resolution level
	static auto blockSize = BLOCK_SIZE_XYZ;

	//static buffer for reading block cache
	static int bufferSize = BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ;
	static byte buffer[BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ];

	static int rowPitch = mFileSize.X;
	static int depthPitch = mFileSize.X * mFileSize.Y;
	
	int bufferPosition = 0;

	auto startPosition = Helper::multiple(blockAddress, VirtualAddress(blockSize));

	//compute the end position range
	//we need to avoid to read the block out of range
	Size endPosition = Size(
		Helper::min(startPosition.X + blockSize, mFileSize.X),
		Helper::min(startPosition.Y + blockSize, mFileSize.Y),
		Helper::min(startPosition.Z + blockSize, mFileSize.Z)
	);

	int length = endPosition.X - startPosition.X;

	//read buffer
	//for volume: it made from surface(index z)
	//for surface: it made from line(index y)
	//so we need to enumerate the z first, it means enumerate the surface of block cache
	//then we need to enumerate the y second, it menas enumerate the line of surface
	//at last, we read the data to buffer
	for (int z = startPosition.Z; z < endPosition.Z; z++) {
		for (int y = startPosition.Y; y < endPosition.Y; y++) {
			//compute the file position offset
			//compute the read block size
			int offset = z * depthPitch + y * rowPitch + startPosition.X;

			assert(bufferPosition + blockSize <= bufferSize);

			//seek position and read
			mFile.seekg(offset);
			mFile.read((char*)&buffer[bufferPosition], length);

			//next
			bufferPosition += blockSize;
		}

		//skip the empty data
		if (endPosition.Y < startPosition.Y + blockSize)
			bufferPosition += blockSize * (startPosition.Y + blockSize - endPosition.Y);
	}

	output = BlockCache(blockSize, buffer);
}

auto VirtualMemoryManager::getPageDirectory() -> GPUPageDirectory *
{
	return mGPUDirectoryCache;
}

auto VirtualMemoryManager::getPageTable() -> GPUPageTable *
{
	return mGPUPageCacheTable;
}

auto VirtualMemoryManager::getBlockTable() -> GPUBlockTable *
{
	return mGPUBlockCacheTable;
}

auto VirtualMemoryManager::getMultiResolutionSizeBuffer() -> ConstantBuffer*
{
	return mMultiResolutionSizeBuffer;
}

auto VirtualMemoryManager::getMultiResolutionBaseBuffer() -> ConstantBuffer*
{
	return mMultiResolutionBaseBuffer;
}

auto VirtualMemoryManager::getMultiResolutionBlockBaseBuffer() -> ConstantBuffer *
{
	return mMultiResolutionBlockBaseBuffer;
}

auto VirtualMemoryManager::getUnorderedAccessUsage() -> std::vector<UnorderedAccessUsage *>
{
	std::vector<UnorderedAccessUsage*> result(2);

	result[0] = mBlockCacheUsageStateUsage;
	result[1] = mBlockCacheMissArrayUsage;

	return result;
}
