#include "GPUPageDirectory.hpp"
#include "GPUPageTable.hpp"

GPUPageDirectory::GPUPageDirectory(Factory * factory, Graphics * graphics, const std::vector<Size>& resolutionSize, GPUPageTable * nextTable)
	: PageDirectory(resolutionSize, nextTable), mGraphics(graphics), mFactory(factory)
{
	//texture size is equal the mSize
	mPageDirectoryTexture = mFactory->createTexture3D(mSize.X, mSize.Y, mSize.Z, PixelFormat::R8G8B8A8Uint, ResourceInfo::ShaderResource());
	mTextureUsage = mFactory->createResourceUsage(mPageDirectoryTexture, mPageDirectoryTexture->getPixelFormat());

	assert(nextTable->mFromTexture == nullptr);
	nextTable->mFromTexture = mPageDirectoryTexture;
}

GPUPageDirectory::~GPUPageDirectory()
{
	mFactory->destroyResourceUsage(mTextureUsage);
	mFactory->destroyTexture3D(mPageDirectoryTexture);
}

void GPUPageDirectory::mapAddress(int resolution, const glm::vec3 & position, BlockCache * blockCache)
{
	//do not need override
	//because all operation will finish in the in the next table
	PageDirectory::mapAddress(resolution, position, blockCache);
}

auto GPUPageDirectory::queryAddress(int resolution, const glm::vec3 & position) -> BlockCache *
{
	//do not need override
	//because all operation will finish in the in the next table
	return PageDirectory::queryAddress(resolution, position);
}

auto GPUPageDirectory::getTexture() const -> Texture3D *
{
	return mPageDirectoryTexture;
}

auto GPUPageDirectory::getTextureUsage() const -> ResourceUsage *
{
	return mTextureUsage;
}
