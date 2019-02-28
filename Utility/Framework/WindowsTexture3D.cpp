#include "WindowsTexture3D.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

WindowsTexture3D::WindowsTexture3D(Graphics * graphics, int width, int height, int depth, PixelFormat pixelFormat,
	const ResourceInfo &info) : Texture3D(graphics, width, height, depth, pixelFormat, info)
{
	D3D11_TEXTURE3D_DESC desc;

	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.Format = Utility::convertPixelFormat(mPixelFormat);
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);
	desc.Width = mWidth;
	desc.Depth = mDepth;

	auto result = static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateTexture3D(&desc, nullptr,
		&mTexture3D);
}

WindowsTexture3D::~WindowsTexture3D()
{
	Utility::dispose(mTexture3D);
}

void WindowsTexture3D::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture3D, 0, nullptr, data, mRowPitch, mDepthPitch);
}

void WindowsTexture3D::update(void * data, int left, int top, int front, int right, int bottom, int back)
{
	D3D11_BOX box;
	box.left = left;
	box.top = top;
	box.front = front;
	box.right = right;
	box.bottom = bottom;
	box.back = back;

	int dataRowPitch = (right - left) * Utility::computePixelFomratBytes(mPixelFormat);
	int dataDepthPitch = dataRowPitch * (bottom - top);

	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture3D, 0, &box, data, dataRowPitch, dataDepthPitch);
}

void WindowsTexture3D::copy(Texture3D * source)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->CopyResource(mTexture3D, static_cast<WindowsTexture3D*>(source)->mTexture3D);
}

auto WindowsTexture3D::map() -> MappedData
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->Map(mTexture3D, 0,
		Utility::convertCpuAccesssFlagToMapType(mResourceInfo.CpuAccessFlag), 0, &mappedResource);

	return MappedData(mappedResource.pData, mappedResource.RowPitch, mappedResource.DepthPitch);
}

void WindowsTexture3D::unmap()
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->Unmap(mTexture3D, 0);
}

#endif // _WIN32
