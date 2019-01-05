#pragma once

#include "ShaderResource.hpp"
#include "PixelFormat.hpp"

#include "Utility.hpp"

class Graphics;

class Texture3D : public ShaderResource {
protected:
	int mWidth;
	int mHeight;
	int mDepth;

	int mRowPitch;
	int mDepthPitch;

	Texture3D(Graphics* graphics, int width, int height, int depth, PixelFormat pixelFormat, BindUsage bindUsage) :
		ShaderResource(graphics, width * height * depth * Utility::ComputePixelFomratBytes(pixelFormat), pixelFormat, bindUsage) {

		mWidth = width;
		mHeight = height;
		mDepth = depth;

		mRowPitch = mWidth * Utility::ComputePixelFomratBytes(pixelFormat);
		mDepthPitch = mWidth * mHeight * Utility::ComputePixelFomratBytes(pixelFormat);
	}

public:
	auto getWidth() -> int const {
		return mWidth;
	}

	auto getHeight() -> int const {
		return mHeight;
	}

	auto getDepth() -> int const {
		return mDepth; 
	}

	virtual void update(void* data, int left, int top, int front, int right, int bottom, int back) = 0;
};