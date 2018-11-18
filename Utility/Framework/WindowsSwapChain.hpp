#pragma once

#include "SwapChain.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsSwapChain : public SwapChain {
private:
	IDXGISwapChain* mSwapChain;

	friend class WindowsRenderTarget;
public:
	WindowsSwapChain(Graphics* graphics, void* outputTarget,
		int width, int height, PixelFormat pixelFormat = PixelFormat::R8G8B8A8Unknown);

	~WindowsSwapChain();

	virtual void present(bool isSync = false)override;
};

#endif // _WIN32
