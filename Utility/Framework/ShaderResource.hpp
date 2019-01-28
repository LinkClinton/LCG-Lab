#pragma once

#include "Resource.hpp"
#include "PixelFormat.hpp"
#include "BindUsage.hpp"


class ShaderResource : public Resource {
protected:
	PixelFormat mPixelFormat;
	
	ShaderResource(Graphics* graphics, int size, PixelFormat pixelFormat, const ResourceInfo &info) : 
		Resource(graphics, size, info), mPixelFormat(pixelFormat) {}

public:
	auto getPixelFormat() -> PixelFormat const {
		return mPixelFormat;
	}
};