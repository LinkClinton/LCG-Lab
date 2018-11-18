#pragma once

#include "ShaderResource.hpp"

#include "Utility.hpp"

class Graphics;

class Texture2D : public ShaderResource {
protected:
	int mWidth;
	int mHeight;
	int mRowPitch;
	
	Texture2D(Graphics* graphics, int width, int height, PixelFormat pixelFormat, BindUsage bindUsage) :
		ShaderResource(graphics, width * height * Utility::ComputePixelFomratBytes(pixelFormat), pixelFormat, bindUsage) {

		mWidth = width;
		mHeight = height;
		mRowPitch = mWidth * Utility::ComputePixelFomratBytes(pixelFormat);
	}

public:
	auto getWidth() -> int const {
		return mWidth;
	}
	
	auto getHeight() -> int const {
		return mHeight;
	}
	
	auto getRowPitch() -> int const {
		return mRowPitch;
	}
};