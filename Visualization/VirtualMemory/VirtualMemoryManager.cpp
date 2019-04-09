#include "VirtualMemoryManager.hpp"

#include <algorithm>

#undef min

void VirtualMemoryManager::analyseFile(const std::string & fileName)
{
	//analyse the volume data
	//we need compute the size of volume and other information
	//to do:

	mFile.sync_with_stdio(false);
	mFile.open(fileName, std::ios::binary);
	mFileSize = Size(800, 800, 800);
}

void VirtualMemoryManager::mapAddressToGPU(int resolution, const glm::vec3 & position, BlockCache * block)
{
	//upload block data to GPU virtual memory
	mGPUDirectoryCache->mapAddress(resolution, position, block);

#ifdef _DEBUG
	static int count = 0;

	printf("Mapped Times from Cpu to Gpu: %d with resolution : %d\n", ++count, resolution);
#endif // _DEBUG

}

void VirtualMemoryManager::initialize(const std::string &fileName, const std::vector<glm::vec3> &resolution)
{
	//initialize the virutal memroy resource(CPU and GPU)
	mResolution = resolution;

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
	
	//initialize resolution 0 directory
	//page size means the size of one page cache can store
	int pageSize = PAGE_SIZE_XYZ * BLOCK_SIZE_XYZ;

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
		auto blockCount = Helper::multiple(directorySize, Size(PAGE_SIZE_XYZ));

		mMultiResolutionSize.push_back(directorySize);
		mMultiResolutionBlockCount.push_back(blockCount.X * blockCount.Y * blockCount.Z);
	}

	mMultiResolutionBase.push_back(0);
	mMultiResolutionBlockBase.push_back(0);

	for (size_t i = 1; i < resolution.size(); i++) {
		auto base = VirtualAddress(mMultiResolutionBase[i - 1].X + mMultiResolutionSize[i - 1].X, 0, 0);
		auto blockBase = mMultiResolutionBlockBase[i - 1] + mMultiResolutionBlockCount[i - 1];

		mMultiResolutionBase.push_back(base);
		mMultiResolutionBlockBase.push_back(blockBase);
		mMultiResolutionBlockEnd.push_back(blockBase - 1);
	}

	//compute the end of block count for multi-resolution
	mMultiResolutionBlockEnd.push_back(
		mMultiResolutionBlockBase[mMultiResolutionBlockBase.size() - 1] +
		mMultiResolutionBlockCount[mMultiResolutionBlockCount.size() - 1] - 1);

	//init page directory cache with multi-resolution
	mDirectoryCache = new PageDirectory(mMultiResolutionSize, mPageCacheTable);

	//compute some information
	for (size_t i = 0; i < mResolution.size(); i++) {
		//the number of voxel in the virtual memory at resolution i
		//the read blocl size in the virtual memory at resolution i
		//the read block size is equal BLOCK_SIZE_XYZ / (voxelCount / file size) = BLOCK_SIZE_XYZ / voxelCount * file size

		auto voxelCount = Helper::multiple(mDirectoryCache->getResolutionSize((int)i), Size(PAGE_SIZE_XYZ * BLOCK_SIZE_XYZ));
		auto readBlockSize = Size(
			(int)((float)BLOCK_SIZE_XYZ / voxelCount.X * mFileSize.X),
			(int)((float)BLOCK_SIZE_XYZ / voxelCount.Y * mFileSize.Y),
			(int)((float)BLOCK_SIZE_XYZ / voxelCount.Z * mFileSize.Z));

		mVoxelCount.push_back(voxelCount);
		mReadBlockSize.push_back(readBlockSize);
	}

	//create buffer for resolution
	mMultiResolutionSizeBuffer = mFactory->createConstantBuffer(sizeof(UInt4) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionBaseBuffer = mFactory->createConstantBuffer(sizeof(UInt4) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionBlockBaseBuffer = mFactory->createConstantBuffer(sizeof(UInt4) * MAX_MULTIRESOLUTION_COUNT, ResourceInfo::ConstantBuffer());
	mMultiResolutionSizeBuffer->update(UInt4::fromVector(mMultiResolutionSize).data());
	mMultiResolutionBaseBuffer->update(UInt4::fromVector(mMultiResolutionBase).data());
	mMultiResolutionBlockBaseBuffer->update(UInt4::fromVector(mMultiResolutionBlockBase).data());

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
	
	int blockCacheCount = 0;

	for (int x = 0; x < mBlockCacheUsageStateTexture->getWidth(); x++) {
		for (int y = 0; y < mBlockCacheUsageStateTexture->getHeight(); y++) {
			for (int z = 0; z < mBlockCacheUsageStateTexture->getDepth(); z++) {
				int id = x + y * usageState.RowPitch + z * usageState.DepthPitch;

				if (usageStateData[id] != 1) continue;

				//query the page table and block table them contain this block
				//trigger the LRU system
				mGPUBlockCacheTable->invertQuery(VirtualAddress(x, y, z));

				blockCacheCount++;
			}
		}
	}

	//unmap
	mBlockCacheUsageStateTexture->unmapCpuTexture();

	//if "blockCacheCount" is equal the block texture size, we do not solve the cache miss
	//because it is meanless
	if (blockCacheCount == BLOCK_COUNT_XYZ * BLOCK_COUNT_XYZ * BLOCK_COUNT_XYZ) return;

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

	int count = 0;

	for (size_t x = 0; x < xEndPosition; x++) {
		for (size_t y = 0; y < yEndPosition; y++) {
			//we store count at (x, y, 0) -> x + y * rowPitch + 0 * depthPitch
			auto cacheMissCount = cacheMissData[x + y * cacheMiss.RowPitch];

			for (size_t z = 1; z <= cacheMissCount; z++) {
				auto id = cacheMissData[x + y * cacheMiss.RowPitch + z * cacheMiss.DepthPitch];

				//get resolution 
				auto resolution = unsigned int(
					std::lower_bound(mMultiResolutionBlockEnd.begin(), mMultiResolutionBlockEnd.end(), id) - mMultiResolutionBlockEnd.begin());
	
				id = id - mMultiResolutionBlockBase[resolution];

				mapAddress(resolution, id); ++count;
			}
		}
	}

#ifdef _DEBUG
	printf("Cache Miss Solved Times Per Frame: %d\n", count);
#endif // _DEBUG


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

	assert(resolution < MAX_MULTIRESOLUTION_COUNT && (size_t)resolution < mResolution.size());

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

	if (mGPUDirectoryCache->queryAddress(resolution, blockCenterPosition) != nullptr) return;

#ifdef _SPARSE_LEAP
	auto type = mSparseLeapManager->tree()->queryNodeType((blockCenterPosition - glm::vec3(0.5f))* mSparseLeapManager->getCube());
#endif // _SPARSE_LEAP


	//query the block if in the CPU virtual memory
	BlockCache* blockCache = mDirectoryCache->queryAddress(resolution, blockCenterPosition);

	//not, we load from disk and add it in to CPU virtual memory
	if (blockCache == nullptr) {
		//load block data from disk and do some special process
		static BlockCache output(BLOCK_SIZE_XYZ);

		bool empty = loadBlock(resolution, blockAddress, output);
		
#ifdef _SPARSE_LEAP
		if (type == OccupancyType::Unknown)
			mSparseLeapManager->tree()->insertNoEmpty(
				(blockCenterPosition - glm::vec3(0.5f)) * mSparseLeapManager->getCube(),
				empty ? OccupancyType::Empty : OccupancyType::NoEmpty);
#endif // _SPARSE_LEAP


		blockCache = &output;

		mDirectoryCache->mapAddress(resolution, blockCenterPosition, blockCache);
	}

	//now, we need upload the block to GPU virtual memory
	mapAddressToGPU(resolution, blockCenterPosition, blockCache);
}

bool VirtualMemoryManager::loadBlock(int resolution, const VirtualAddress & blockAddress, BlockCache & output) 
{
	//because of the resolution, the size of block is not equal the BLOCK_SIZE_XYZ
	//in other word, it may be bigger(smaller) than BLOCK_SIZE_XYZ
	//we need to scale it to block that its size is BLOCK_SIZE_XYZ
	//in this version, we only skip some voxel or sample same voxel to scale it

	//get read block size
	auto readBlockSize = mReadBlockSize[resolution];

	//get read block entry, it is equal block address * read block size
	auto readBlockEntry = Helper::multiple(blockAddress, readBlockSize);

	//z and y offset
	float xOffset = (float)(readBlockSize.X - 1) / (BLOCK_SIZE_XYZ - 1);
	float zOffset = (float)(readBlockSize.Z - 1) / (BLOCK_SIZE_XYZ - 1);
	float yOffset = (float)(readBlockSize.Y - 1) / (BLOCK_SIZE_XYZ - 1);

	int fileRowPitch = mFileSize.X;
	int fileDepthPitch = fileRowPitch * mFileSize.Y;

	int blockRowPitch = BLOCK_SIZE_XYZ;
	int blockDepthPitch = blockRowPitch * BLOCK_SIZE_XYZ;

	static byte buffer[MAX_READ_BUFFER];

	unsigned long long average = 0;

	//read block
	float zPosition = (float)readBlockEntry.Z;

	for (int zCount = 0; zCount < BLOCK_SIZE_XYZ; zCount++, zPosition += zOffset) {
		float yPosition = (float)readBlockEntry.Y;

		for (int yCount = 0; yCount < BLOCK_SIZE_XYZ; yCount++, yPosition += yOffset) {
			//get the start position in the file we need read
			int readStartPosition =
				(int)std::round(zPosition) * fileDepthPitch +
				(int)std::round(yPosition) * fileRowPitch +
				readBlockEntry.X;

			//get the start position in the block we need copy to
			int blockStartPosition = zCount * blockDepthPitch + yCount * blockRowPitch;

			//the memory address we need copy to
			byte* address = output.getDataPointer() + blockStartPosition;

			//read data
			mFile.seekg(readStartPosition);
			mFile.read((char*)buffer, readBlockSize.X);

			float xPosition = 0;

			//copy data
			for (int xCount = 0; xCount < BLOCK_SIZE_XYZ; xCount++, xPosition += xOffset)
				average = average + (address[xCount] = buffer[(int)std::round(xPosition)]);
		}
	}

	average = average / (BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ * BLOCK_SIZE_XYZ);

	if (average / 255.0f >= EMPTY_LIMIT) return false;
	return true;
}

auto VirtualMemoryManager::detectResolutionLevel(float ratio) -> int
{
	static auto gpuBlockCount = (unsigned int)(BLOCK_COUNT_XYZ * BLOCK_COUNT_XYZ * BLOCK_COUNT_XYZ);

	for (size_t i = 0; i < mMultiResolutionBlockCount.size(); i++) {
		unsigned int blockCount = mMultiResolutionBlockCount[i];
		unsigned int requirement = (unsigned int)ceil(blockCount * ratio);

		if (gpuBlockCount >= requirement) return (int)i;
	}

	return (int)(mMultiResolutionBlockCount.size() - 1);
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
