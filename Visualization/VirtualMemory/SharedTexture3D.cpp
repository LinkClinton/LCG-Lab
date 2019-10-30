#include "SharedTexture3D.hpp"

SharedTexture3D::SharedTexture3D(int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo & info, Factory * factory)
	:mFactory(factory)
{
	//cpu texture in the staging heap
	//cpu access flag is read and write((Read | Write) == 3)
	static ResourceInfo cpuResourceInfo = 
		ResourceInfo(BindUsage::None, CpuAccessFlag(3), HeapType::Staging);

	mGpuTexture = mFactory->createTexture3D(width, height, depth, pixelFormat, info);
	mCpuTexture = mFactory->createTexture3D(width, height, depth, pixelFormat, cpuResourceInfo);
}

SharedTexture3D::~SharedTexture3D()
{
	mFactory->destroyTexture3D(mGpuTexture);
	mFactory->destroyTexture3D(mCpuTexture);
}

void SharedTexture3D::update() const {
	//copy resource
	mCpuTexture->copy(mGpuTexture);
}

auto SharedTexture3D::mapCpuTexture() const -> MappedData
{
	return mCpuTexture->map();
}

void SharedTexture3D::unmapCpuTexture() const {
	mCpuTexture->unmap();
}

auto SharedTexture3D::getWidth() const -> int
{
	return mCpuTexture->getWidth();
}

auto SharedTexture3D::getHeight() const -> int
{
	return mCpuTexture->getHeight();
}

auto SharedTexture3D::getDepth() const -> int
{
	return mCpuTexture->getDepth();
}

auto SharedTexture3D::getPixelFormat() const -> PixelFormat
{
	return mCpuTexture->getPixelFormat();
}

auto SharedTexture3D::getGpuTexture() const -> Texture3D *
{
	return mGpuTexture;
}

auto SharedTexture3D::getCpuTexture() const -> Texture3D *
{
	return mCpuTexture;
}
