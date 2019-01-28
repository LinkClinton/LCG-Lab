#include "WindowsSwapChain.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsRenderTarget.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsSwapChain::WindowsSwapChain(Graphics * graphics, void * outputTarget, int width, int height, PixelFormat pixelFormat)
	:SwapChain(outputTarget, width, height, pixelFormat)
{
	DXGI_SWAP_CHAIN_DESC swapDesc;

	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Format = (DXGI_FORMAT)Utility::convertPixelFormat(mPixelFormat);
	swapDesc.BufferDesc.Height = mHeight;
	swapDesc.BufferDesc.Width = mWidth;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = (HWND)outputTarget;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;

	IDXGIDevice* device = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIFactory* factory = nullptr;

	static_cast<WindowsGraphics*>(graphics)->mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&device));
	device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&adapter));
	adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));

	factory->CreateSwapChain(static_cast<WindowsGraphics*>(graphics)->mDevice, &swapDesc, &mSwapChain);

	Utility::dispose(device);
	Utility::dispose(adapter);
	Utility::dispose(factory);

	mRenderTarget = new WindowsRenderTarget(graphics, this);
}

WindowsSwapChain::~WindowsSwapChain()
{
	Utility::dispose(mSwapChain);
	Utility::Delete(mRenderTarget);
}

void WindowsSwapChain::present(bool isSync)
{
	mSwapChain->Present(isSync ? 1 : 0, 0);
}

#endif // _WIN32