#include "WindowsDepthStencil.hpp"
#include "WindowsTexture2D.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsDepthStencil::WindowsDepthStencil(Graphics * graphics, Texture2D * texture2D, PixelFormat pixelFormat)
	: DepthStencil(texture2D, pixelFormat)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;

	desc.Flags = 0;
	desc.Format = (DXGI_FORMAT)Utility::ConvertPixelFormat(mPixelFormat);
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;

	auto result = static_cast<WindowsGraphics*>(graphics)->mDevice->CreateDepthStencilView(
		static_cast<WindowsTexture2D*>(texture2D)->mTexture2D, &desc, &mDepthStencil);
}

WindowsDepthStencil::~WindowsDepthStencil()
{
	Utility::Dispose(mDepthStencil);
}

#endif // _WIN32

