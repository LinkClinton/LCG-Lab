#include "WindowsTexture2D.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

WindowsTexture2D::WindowsTexture2D(Graphics * graphics, int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) :
	Texture2D(graphics, width, height, pixelFormat, info)
{
	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.Format = Utility::convertPixelFormat(mPixelFormat);
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);
	desc.Width = mWidth;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateTexture2D(&desc, nullptr,
		&mTexture2D);
}

WindowsTexture2D::~WindowsTexture2D()
{
	Utility::dispose(mTexture2D);
}

void WindowsTexture2D::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture2D, 0, nullptr, data, mRowPitch, 0);
}

#endif // _WIN32