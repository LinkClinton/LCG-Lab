#include "GPUPageTable.hpp"

std::vector<byte> GPUPageTable::mPageCacheClearMemory;

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUPageTable * nextTable)
	: PageTable(size, nextTable), mFactory(factory), mGraphics(graphics), mFromTexture(nullptr)
{
	//texture size is equal the table size * block size
	auto textureSize = Helper::multiple(mSize, PageCache::getPageCacheSize());

	mPageTableTexture = mFactory->createTexture3D(textureSize.X, textureSize.Y, textureSize.Z, PixelFormat::R8G8B8A8Unknown);
	mTextureUsage = mFactory->createResourceUsage(mPageTableTexture, mPageTableTexture->getPixelFormat());

	assert(nextTable->mFromTexture == nullptr);
	nextTable->mFromTexture = mPageTableTexture;
}

GPUPageTable::GPUPageTable(Factory * factory, Graphics * graphics, const Size & size, GPUBlockTable * endTable)
	: PageTable(size, endTable), mFactory(factory), mGraphics(graphics), mFromTexture(nullptr)
{
	//texture size is equal the table size * block size
	auto textureSize = Helper::multiple(mSize, PageCache::getPageCacheSize());

	mPageTableTexture = mFactory->createTexture3D(textureSize.X, textureSize.Y, textureSize.Z, PixelFormat::R8G8B8A8Unknown);
	mTextureUsage = mFactory->createResourceUsage(mPageTableTexture, mPageTableTexture->getPixelFormat());

	assert(endTable->mFromTexture == nullptr);
	endTable->mFromTexture = mPageTableTexture;
}

GPUPageTable::~GPUPageTable()
{
	mFactory->destoryResourceUsage(mTextureUsage);
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
	
	//clear page cache(GPU version, texture)
	auto size = PageCache::getPageCacheSize();
	auto startRange = Helper::multiple(size, address);
	auto textureSize = size.X * size.Y * size.Z * Utility::ComputePixelFomratBytes(mPageTableTexture->getPixelFormat());
	
	//reset the clear memory(all zero)
	if (mPageCacheClearMemory.size() != (size_t)textureSize) mPageCacheClearMemory.resize(textureSize);

	//update memory to texture
	mPageTableTexture->update(&mPageCacheClearMemory[0],
		startRange.X, startRange.Y, startRange.Z,
		startRange.X + size.X, startRange.Y + size.Y, startRange.Z + size.Z);

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

auto GPUPageTable::getTexture() -> Texture3D *
{
	return mPageTableTexture;
}

auto GPUPageTable::getTextureUsage() -> ResourceUsage *
{
	return mTextureUsage;
}
