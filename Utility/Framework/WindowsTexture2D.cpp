#include "WindowsTexture2D.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

WindowsTexture2D::WindowsTexture2D(Graphics * graphics, int width, int height, PixelFormat pixelFormat, BindUsage bindUsage) :
	Texture2D(graphics, width, height, pixelFormat, bindUsage)
{
	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = Utility::ConvertBindUsage(bindUsage) | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.Format = (DXGI_FORMAT)Utility::ConvertPixelFormat(pixelFormat);
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc.Width = mWidth;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateTexture2D(&desc, nullptr,
		&mTexture2D);
}

WindowsTexture2D::~WindowsTexture2D()
{
	Utility::Dispose(mTexture2D);
}

void WindowsTexture2D::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture2D, 0, nullptr, data, mRowPitch, 0);
}

#endif // _WIN32