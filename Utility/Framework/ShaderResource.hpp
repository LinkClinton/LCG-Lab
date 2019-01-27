#pragma once

#include "Resource.hpp"
#include "PixelFormat.hpp"
#include "BindUsage.hpp"


class ShaderResource : public Resource {
protected:
	PixelFormat mPixelFormat;
	BindUsage mBindUsage;

	ShaderResource(Graphics* graphics, int size, PixelFormat pixelFormat, BindUsage bindUsage, HeapType heapType) : 
		Resource(graphics, size, heapType), mPixelFormat(pixelFormat), mBindUsage(bindUsage) {}

public:
	auto getPixelFormat() -> PixelFormat const {
		return mPixelFormat;
	}

	auto getBindUsage() -> BindUsage const {
		return mBindUsage;
	}
};