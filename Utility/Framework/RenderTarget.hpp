#pragma once

#include "PixelFormat.hpp"
#include "SwapChain.hpp"
#include "Texture2D.hpp"

class RenderTarget {
protected:
	int mWidth;
	int mHeight;

	PixelFormat mPixelFormat;

	RenderTarget(SwapChain* swapChain) {
		mWidth = swapChain->getWidth();
		mHeight = swapChain->getHeight();
		mPixelFormat = swapChain->getPixelFormat();
	}

	RenderTarget(Texture2D* texture2D, PixelFormat pixelFormat) {
		mWidth = texture2D->getWidth();
		mHeight = texture2D->getHeight();
		mPixelFormat = pixelFormat;
	}
public:
	auto getWidth() -> int const {
		return mWidth;
	}

	auto getHeight() -> int const {
		return mHeight;
	}

	auto getPixelFormat() -> PixelFormat const {
		return mPixelFormat;
	}
};