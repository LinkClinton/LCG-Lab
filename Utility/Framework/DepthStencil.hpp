#pragma once

#include "PixelFormat.hpp"
#include "Texture2D.hpp"

class DepthStencil {
protected:
	int mWidth;
	int mHeight;

	PixelFormat mPixelFormat;

	DepthStencil(Texture2D* texture2D, PixelFormat pixelFormat) {
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