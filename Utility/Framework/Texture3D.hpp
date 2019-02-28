#pragma once

#include "ShaderResource.hpp"
#include "PixelFormat.hpp"

#include "Utility.hpp"
#include "MappedData.hpp"

class Graphics;

class Texture3D : public ShaderResource {
protected:
	int mWidth;
	int mHeight;
	int mDepth;

	int mRowPitch;
	int mDepthPitch;

	Texture3D(Graphics* graphics, int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info) :
		ShaderResource(graphics, width * height * depth * Utility::computePixelFomratBytes(pixelFormat), pixelFormat, info) {

		mWidth = width;
		mHeight = height;
		mDepth = depth;

		mRowPitch = mWidth * Utility::computePixelFomratBytes(pixelFormat);
		mDepthPitch = mWidth * mHeight * Utility::computePixelFomratBytes(pixelFormat);
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

	virtual void copy(Texture3D* source) = 0;

	virtual auto map() -> MappedData = 0;

	virtual void unmap() = 0;
};