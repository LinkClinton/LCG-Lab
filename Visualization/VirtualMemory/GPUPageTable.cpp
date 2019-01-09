#include "GPUPageTable.hpp"

#include "SharedMacro.hpp"

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUPageTable * nextTable)
	: PageTable(size, nextTable), mFactory(factory), mGraphics(graphics), mFromTexture(nullptr)
{
	//texture size is equal the table size * block size
	auto textureSize = Helper::multiple(mSize, PageCache::getPageCacheSize());

	mPageTableTexture = mFactory->createTexture3D(textureSize.X, textureSize.Y, textureSize.Z, PixelFormat::R8G8B8A8Unknown);

	assert(nextTable->mFromTexture == nullptr);
	nextTable->mFromTexture = mPageTableTexture;
}

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUBlockTable * endTable)
	: PageTable(size, endTable), mFactory(factory), mGraphics(graphics), mFromTexture(nullptr)
{
	//texture size is equal the table size * block size
	auto textureSize = Helper::multiple(mSize, PageCache::getPageCacheSize());

	mPageTableTexture = mFactory->createTexture3D(textureSize.X, textureSize.Y, textureSize.Z, PixelFormat::R8G8B8A8Unknown);

	assert(endTable->mFromTexture == nullptr);
	endTable->mFromTexture = mPageTableTexture;
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
	GPUHelper::modifyVirtualLinkToTexture(virtualLink, mFromTexture);
}

void GPUPageTable::clearUpAddress(const VirtualAddress & address)
{
	//before CPU version, we need to clear up the relationship in the texture(GPU memory)
	//we get the virtual link from page table to block table
	//because the virtual link will be removed by calling the PageTable::clearUpAddress
	auto virtualLink = mMapRelation[getArrayIndex(address)];

	PageTable::clearUpAddress(address);

	//now, the virtual link is reset, we need upload it to the texture
	if (virtualLink != nullptr) GPUHelper::modifyVirtualLinkToTexture(virtualLink, mFromTexture);
}

void GPUPageTable::mapAddress(const glm::vec3 & position, const Size & size, BlockCache * blockCache, VirtualLink * virtualLink)
{
	//do not override
	PageTable::mapAddress(position, size, blockCache, virtualLink);
}

auto GPUPageTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache * 
{
	//do not override
	return PageTable::queryAddress(position, size, virtualLink);
}
