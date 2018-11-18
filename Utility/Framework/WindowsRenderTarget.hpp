#pragma once

#include "RenderTarget.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;
class SwapChain;

class WindowsRenderTarget : public RenderTarget {
private:
	ID3D11RenderTargetView* mRenderTarget;

	friend class WindowsGraphics;
public:
	WindowsRenderTarget(Graphics* graphics, SwapChain* swapChain);

	WindowsRenderTarget(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat);

	~WindowsRenderTarget();
};

#endif // _WIN32
