#pragma once

#include "PixelFormat.hpp"

class Graphics;

class RenderTarget;

class SwapChain {
protected:
	int mWidth;
	int mHeight;

	void* mOutputTarget;

	PixelFormat mPixelFormat;

	RenderTarget* mRenderTarget;

	SwapChain(void* outputTarget, int width, int height, PixelFormat pixelFormat = PixelFormat::R8G8B8A8Unknown): 
		mWidth(width), mHeight(height), mOutputTarget(outputTarget), mPixelFormat(pixelFormat), mRenderTarget(nullptr) {};
public:

	virtual void present(bool isSync = false) = 0;

	auto getWidth() -> int const {
		return mWidth;
	}

	auto getHeight() -> int const {
		return mHeight;
	}

	auto getOutputTarget() -> void* const {
		return mOutputTarget;
	}

	auto getPixelFormat() -> PixelFormat const {
		return mPixelFormat;
	}

	auto getRenderTarget() -> RenderTarget* const {
		return mRenderTarget;
	}
};