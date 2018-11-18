#include "WindowsRenderTarget.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsSwapChain.hpp"
#include "WindowsTexture2D.hpp"


#include "Utility.hpp"

#ifdef _WIN32

WindowsRenderTarget::WindowsRenderTarget(Graphics * graphics, SwapChain * swapChain)
	: RenderTarget(swapChain)
{
	ID3D11Texture2D* backBuffer = nullptr;

	static_cast<WindowsSwapChain*>(swapChain)->mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	D3D11_RENDER_TARGET_VIEW_DESC desc;

	desc.Format = (DXGI_FORMAT)Utility::ConvertPixelFormat(mPixelFormat);
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;

	static_cast<WindowsGraphics*>(graphics)->mDevice->CreateRenderTargetView(backBuffer, &desc, &mRenderTarget);

	Utility::Dispose(backBuffer);
}

WindowsRenderTarget::WindowsRenderTarget(Graphics * graphics, Texture2D * texture2D, PixelFormat pixelFormat)
	: RenderTarget(texture2D, pixelFormat)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;

	desc.Format = (DXGI_FORMAT)Utility::ConvertPixelFormat(mPixelFormat);
	desc.Texture2D.MipSlice = 0;
	desc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;

	static_cast<WindowsGraphics*>(graphics)->mDevice->CreateRenderTargetView(
		static_cast<WindowsTexture2D*>(texture2D)->mTexture2D, &desc,
		&mRenderTarget
	);
}

WindowsRenderTarget::~WindowsRenderTarget()
{
	Utility::Dispose(mRenderTarget);
}

#endif // _WIN32
