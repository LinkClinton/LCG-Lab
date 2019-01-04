#include "GPUPageTable.hpp"

#include "SharedMacro.hpp"

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUPageTable * nextTable)
	: PageTable(size, nextTable), mFactory(factory), mGraphics(graphics)
{
	int pageTextureSize = PAGE_COUNT_XYZ * PAGE_SIZE_XYZ;

	mPageTableTexture = mFactory->createTexture3D(pageTextureSize, pageTextureSize, pageTextureSize, PixelFormat::R8G8B8A8Unknown);
}

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUBlockTable * endTable)
	: PageTable(size, endTable), mFactory(factory), mGraphics(graphics)
{
	int pageTextureSize = PAGE_COUNT_XYZ * PAGE_SIZE_XYZ;

	mPageTableTexture = mFactory->createTexture3D(pageTextureSize, pageTextureSize, pageTextureSize, PixelFormat::R8G8B8A8Unknown);
}

GPUPageTable::~GPUPageTable()
{
	mFactory->destoryTexture3D(mPageTableTexture);
}

void GPUPageTable::mallocAddress(VirtualLink * virtualLink)
{
	//CPU
	PageTable::mallocAddress(virtualLink);

	//now we need to update the relationship in the texture(GPU memory)
	//to do:
}

void GPUPageTable::clearUpAddress(const VirtualAddress & address)
{
	//before CPU version, we need to clear up the relationship in the texture(GPU memory)
	//to do:

	PageTable::clearUpAddress(address);
}
