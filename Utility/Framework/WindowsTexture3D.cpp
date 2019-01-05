#include "WindowsTexture3D.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

WindowsTexture3D::WindowsTexture3D(Graphics * graphics, int width, int height, int depth, PixelFormat pixelFormat,
	BindUsage bindUsage) : Texture3D(graphics, width, height, depth, pixelFormat, bindUsage)
{
	D3D11_TEXTURE3D_DESC desc;

	desc.BindFlags = Utility::ConvertBindUsage(mBindUsage) | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.Format = (DXGI_FORMAT)Utility::ConvertPixelFormat(pixelFormat);
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc.Width = mWidth;
	desc.Depth = mDepth;

	auto result = static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateTexture3D(&desc, nullptr,
		&mTexture3D);
}

WindowsTexture3D::~WindowsTexture3D()
{
	Utility::Dispose(mTexture3D);
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

	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture3D, 0, &box, data, mRowPitch, mDepthPitch);
}

#endif // _WIN32
