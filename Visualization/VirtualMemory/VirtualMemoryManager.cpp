#include "VirtualMemoryManager.hpp"

void VirtualMemoryManager::analyseFile(const std::string & fileName)
{
	//analyse the volume data
	//we need compute the size of volume and other information
	//to do

	mFile.sync_with_stdio(false);
	mFile.open(fileName, std::ios::binary);
}

void VirtualMemoryManager::mapAddressToGPU(BlockCache * block, int resolutionLevel, const glm::vec3 & position)
{
	//upload block data to GPU virtual memory
	//for current version, we only have one page
	//first, get the block we can cover from LRU system
}

void VirtualMemoryManager::initialize(const std::string &fileName, const std::vector<glm::vec3> &resolution)
{
	//initialize the virutal memroy resource(CPU and GPU)

	//expand size of CPU virtual memory
	//if the size of CPU and GPU are same, it can not work well
	const int expand = 2;

	analyseFile(fileName);
	
	//set cache size
	PageCache::setPageCacheSize(Size(PAGE_SIZE_XYZ));
	BlockCache::setBlockCacheSize(Size(BLOCK_SIZE_XYZ));

	//for CPU virtual memory we expand the size of block and page table
	mBlockCacheTable = new BlockTable(Size(BLOCK_COUNT_XYZ + expand));
	mPageCacheTable = new PageTable(Size(PAGE_COUNT_XYZ + expand), mBlockCacheTable);
	
	assert(resolution.size() == ((size_t)MULTIRESOLUTION_COUNT));

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

		mDirectoryCache[i] = new PageDirectory(directorySize, mPageCacheTable);
	}

	//initialize texture for gpu virtual memory
	int blockTextureSize = BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ;
	int pageTextureSize = PAGE_COUNT_XYZ * PAGE_SIZE_XYZ;

	mBlockCacheTexture = mFactory->createTexture3D(blockTextureSize, blockTextureSize, blockTextureSize, PixelFormat::R8Unknown);
	mPageCacheTexture = mFactory->createTexture3D(pageTextureSize, pageTextureSize, pageTextureSize, PixelFormat::R8G8B8A8Unknown);
	
	//because we need to store the directory for multi-resolution
	//so we need store all directories to one texture
	//the y and z(height and depth) size are the biggest size of directory(resolution 0)
	//the x(width) size is the sum of directory's x size
	int directoryTextureYSize = mDirectoryCache[0]->getSize().Y;
	int directoryTextureZSize = mDirectoryCache[0]->getSize().Z;
	int directoryTextureXSize = 0;

	for (int i = 0; i < MULTIRESOLUTION_COUNT; i++) directoryTextureXSize += mDirectoryCache[i]->getSize().X;

	mDirectoryCacheTexture = mFactory->createTexture3D(directoryTextureXSize, directoryTextureYSize, directoryTextureZSize, PixelFormat::R8G8B8A8Unknown);

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

	mBlockCacheUsageStateTexture = mFactory->createTexture3D(blockCacheUsageStateSize, blockCacheUsageStateSize, blockCacheUsageStateSize, PixelFormat::R8Unknown);
	mBlockCacheMissArrayTexture = mFactory->createTexture3D(blockCacheMissArraySizeX, blockCacheMissArraySizeY, blockCacheMissArraySizeZ, PixelFormat::R32G32Uint);

	//initialize the usage
	mDirectoryCacheUsage = mFactory->createResourceUsage(mDirectoryCacheTexture, mDirectoryCacheTexture->getPixelFormat());
	mPageCacheUsage = mFactory->createResourceUsage(mPageCacheTexture, mPageCacheTexture->getPixelFormat());
	mBlockCacheUsage = mFactory->createResourceUsage(mBlockCacheTexture, mBlockCacheTexture->getPixelFormat());

	mBlockCacheUsageStateUsage = mFactory->createUnorderedAccessUsage(mBlockCacheUsageStateTexture, mBlockCacheUsageStateTexture->getPixelFormat());
	mBlockCacheMissArrayUsage = mFactory->createUnorderedAccessUsage(mBlockCacheMissArrayTexture, mBlockCacheMissArrayTexture->getPixelFormat());
}

void VirtualMemoryManager::mapAddress(int resolutionLevel, int blockID) 
{
	//for each cache miss, we will test if the block in the CPU virtual memory
	//if it is not in the memory, we will load it from disk
	//if it is in the memory, we will upload it to GPU virtual memory

	assert(resolutionLevel < MULTIRESOLUTION_COUNT);

	//get directory cache and it's size for this resolution level
	auto directoryCache = mDirectoryCache[resolutionLevel];
	auto directoryCacheSize = directoryCache->getSize();

	//if block is legal
	assert(blockID >= 0 && blockID < directoryCacheSize.X * directoryCacheSize.Y * directoryCacheSize.Z);

	//row pitch is equal width and depth pitch is equal (width * height)
	auto rowPitch = directoryCacheSize.X;
	auto depthPitch = directoryCacheSize.X * directoryCacheSize.Y;

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
		(blockAddress.X + 0.5f) / directoryCacheSize.X,
		(blockAddress.Y + 0.5f) / directoryCacheSize.Y,
		(blockAddress.Z + 0.5f) / directoryCacheSize.Z
	);

	//query the block if in the CPU virtual memory
	BlockCache* blockCache = directoryCache->queryAddress(blockCenterPosition);

	//not, we load from disk and add it in to CPU virtual memory
	if (blockCache == nullptr) {
		//load block data from disk and do some special process
		blockCache = loadBlock(resolutionLevel, blockAddress);
		
		directoryCache->mapAddress(blockCenterPosition, blockCache);
	}

	//now, we need upload the block to GPU virtual memory
	mapAddressToGPU(blockCache, resolutionLevel, blockCenterPosition);
}

auto VirtualMemoryManager::loadBlock(int resolutionLevel, const VirtualAddress & blockAddress) -> BlockCache *
{
	return nullptr;
}
