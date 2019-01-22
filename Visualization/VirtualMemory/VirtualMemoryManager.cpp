#include "VirtualMemoryManager.hpp"

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

	std::vector<Size> multiResolutionSize;
	std::vector<VirtualAddress> multiResolutionBase;

	//for all resolution, we compute the real size
	//then we compute the directory size we need
	for (size_t i = 0; i < resolution.size(); i++) {
		auto realSize = Helper::multiple(mFileSize, resolution[i]);
		auto directorySize = Size(
			(int)ceil((float)realSize.X / pageSize),
			(int)ceil((float)realSize.Y / pageSize),
			(int)ceil((float)realSize.Z / pageSize)
		);

		multiResolutionSize.push_back(directorySize);
	}

	multiResolutionBase.push_back(0);

	for (size_t i = 1; i < resolution.size(); i++) {
		auto base = VirtualAddress(multiResolutionBase[i - 1].X + multiResolutionSize[i - 1].X, 0, 0);

		multiResolutionBase.push_back(base);
	}

	//init page directory cache with multi-resolution
	mDirectoryCache = new PageDirectory(multiResolutionSize, mPageCacheTable);

	//create buffer for resolution
	mMultiResolutionSizeBuffer = mFactory->createConstantBuffer(sizeof(Size) * MAX_MULTIRESOLUTION_COUNT);
	mMultiResolutionBaseBuffer = mFactory->createConstantBuffer(sizeof(VirtualAddress) * MAX_MULTIRESOLUTION_COUNT);
	mMultiResolutionSizeBuffer->update(&multiResolutionSize[0]);
	mMultiResolutionBaseBuffer->update(&multiResolutionBase[0]);

	//init the GPU resource(Texture3D and ResourceUsage)
	mGPUBlockCacheTable = new GPUBlockTable(mFactory, mGraphics, BLOCK_COUNT_XYZ);
	mGPUPageCacheTable = new GPUPageTable(mFactory, mGraphics, PAGE_COUNT_XYZ, mGPUBlockCacheTable);
	mGPUDirectoryCache = new GPUPageDirectory(mFactory, mGraphics, multiResolutionSize, mGPUPageCacheTable);

	//for current version, we use one byte to store a block state(it is simple, may be changed in next version) 
	//and we do not use hash to avoid the same block problem(will be solved in next version)
	//and we report resolution level and block id for cache miss(will be one block id for next version)
	//for block cache miss texture, we need compute the x and y size and make sure it can cover the render target
	//so the x and y size is equal "ceil(mResolutionSize / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY)"
	//and the z size means the array size(0 for count)
	int blockCacheUsageStateSize = BLOCK_COUNT_XYZ;
	int blockCacheMissArraySizeX = (int)ceil((float)mResolutionWidth / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY);
	int blockCacheMissArraySizeY = (int)ceil((float)mResolutionHeight / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY);
	int blockCacheMissArraySizeZ = BLOCK_HASH_TABLE_ARRAY_SIZE;

	//create texture 3d and resource usage
	mBlockCacheUsageStateTexture = mFactory->createTexture3D(blockCacheUsageStateSize, blockCacheUsageStateSize, blockCacheUsageStateSize, PixelFormat::R8Unknown);
	mBlockCacheMissArrayTexture = mFactory->createTexture3D(blockCacheMissArraySizeX, blockCacheMissArraySizeY, blockCacheMissArraySizeZ, PixelFormat::R32Uint);

	mBlockCacheUsageStateUsage = mFactory->createUnorderedAccessUsage(mBlockCacheUsageStateTexture, mBlockCacheUsageStateTexture->getPixelFormat());
	mBlockCacheMissArrayUsage = mFactory->createUnorderedAccessUsage(mBlockCacheMissArrayTexture, mBlockCacheMissArrayTexture->getPixelFormat());
}

void VirtualMemoryManager::finalize() 
{
	delete mDirectoryCache;
	delete mPageCacheTable;
	delete mBlockCacheTable;

	delete mGPUDirectoryCache;
	delete mGPUPageCacheTable;
	delete mGPUBlockCacheTable;

	mFactory->destoryTexture3D(mBlockCacheUsageStateTexture);
	mFactory->destoryTexture3D(mBlockCacheMissArrayTexture);

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
		(blockID % depthPitch),
		(blockID % depthPitch) / rowPitch,
		(blockID % depthPitch) % rowPitch
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
		blockCache = loadBlock(resolution, blockAddress);
		
		mDirectoryCache->mapAddress(resolution, blockCenterPosition, blockCache);
	}

	//now, we need upload the block to GPU virtual memory
	mapAddressToGPU(resolution, blockCenterPosition, blockCache);
}

auto VirtualMemoryManager::loadBlock(int resolution, const VirtualAddress & blockAddress) -> BlockCache *
{
	//to do:
	return nullptr;
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

auto VirtualMemoryManager::getUnorderedAccessUsage() -> std::vector<UnorderedAccessUsage *>
{
	std::vector<UnorderedAccessUsage*> result(2);

	result[0] = mBlockCacheUsageStateUsage;
	result[1] = mBlockCacheMissArrayUsage;

	return result;
}
