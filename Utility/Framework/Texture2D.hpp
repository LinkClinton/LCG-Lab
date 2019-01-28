#pragma once

#include "ShaderResource.hpp"

#include "Utility.hpp"

class Graphics;

class Texture2D : public ShaderResource {
protected:
	int mWidth;
	int mHeight;
	int mRowPitch;
	
	Texture2D(Graphics* graphics, int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) :
		ShaderResource(graphics, width * height * Utility::computePixelFomratBytes(pixelFormat), pixelFormat, info) {

		mWidth = width;
		mHeight = height;
		mRowPitch = mWidth * Utility::computePixelFomratBytes(pixelFormat);
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