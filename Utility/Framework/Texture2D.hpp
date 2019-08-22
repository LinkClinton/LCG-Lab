#pragma once

#include "ShaderResource.hpp"

#include "Utility.hpp"
#include "MappedData.hpp"

class Graphics;

class Texture2D : public ShaderResource {
protected:
	int mWidth;
	int mHeight;
	int mRowPitch;
	
	Texture2D(Graphics* graphics, int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) :
		ShaderResource(graphics, width * height * Utility::computePixelFormatBytes(pixelFormat), pixelFormat, info) {

		mWidth = width;
		mHeight = height;
		mRowPitch = mWidth * Utility::computePixelFormatBytes(pixelFormat);
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

	virtual auto map()-> MappedData = 0;

	virtual void unmap() = 0;

	virtual void save(const std::wstring& fileName) = 0;
};