#include "GPUBlockTable.hpp"

#include "SharedMacro.hpp"

GPUBlockTable::GPUBlockTable(Factory * factory, Graphics * graphics, const Size & size) :
	BlockTable(size), mFactory(factory), mGraphics(graphics)
{
	//initialize texture(block cache) for gpu virtual memory
	int blockTextureSize = BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ;
	int pageTextureSize = PAGE_COUNT_XYZ * PAGE_SIZE_XYZ;

	mBlockTableTexture = mFactory->createTexture3D(blockTextureSize, blockTextureSize, blockTextureSize, PixelFormat::R8Unknown);
}

GPUBlockTable::~GPUBlockTable()
{
	mFactory->destoryTexture3D(mBlockTableTexture);
}

void GPUBlockTable::mallocAddress(VirtualLink * virtualLink)
{
	//CPU
	BlockTable::mallocAddress(virtualLink);

	//now we need to update the relationship in the texture(GPU memory)
	//to do:
}

void GPUBlockTable::clearUpAddress(const VirtualAddress & address)
{
	//before CPU version, we need to clear up the relationship in the texture(GPU memory)
	//to do:

	BlockTable::clearUpAddress(address);
}

void GPUBlockTable::mapAddress(const glm::vec3 & position, const Size & size, BlockCache * blockCache, VirtualLink * virtualLink)
{
	//in fact, we can not to set the block cache in the CPU version
	//because we will delete it not only once
	//so we only upload the block cache to the texture(GPU memory) and set virtual empty block cache
	//to do:


	//CPU version
	//set virtual empty block cache, because the block cache only need to upload to texture(GPU memory)
	setAddress(virtualLink->Address, new BlockCache(Size(0, 0, 0), VirtualAddress(0, 0, 0), nullptr));
}

auto GPUBlockTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache * 
{
	//do not need override
	return BlockTable::queryAddress(position, size, virtualLink);
}
