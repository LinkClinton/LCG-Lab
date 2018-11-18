#pragma once

#include "Resource.hpp"
#include "PixelFormat.hpp"
#include "BindUsage.hpp"


class ShaderResource : public Resource {
protected:
	PixelFormat mPixelFormat;
	BindUsage mBindUsage;

	ShaderResource(Graphics* graphics, int size, PixelFormat pixelFormat, BindUsage bindUsage) : 
		Resource(graphics, size), mPixelFormat(pixelFormat), mBindUsage(bindUsage) {}

public:
	auto getPixelFormat() -> PixelFormat const {
		return mPixelFormat;
	}

	auto getBindUsage() -> BindUsage const {
		return mBindUsage;
	}
};