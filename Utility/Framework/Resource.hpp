#pragma once

class Graphics;

#include "ResourceInfo.hpp"

class Resource {
protected:
	int mSize;

	ResourceInfo mResourceInfo;
	
	Graphics* mGraphics;

	Resource(Graphics* graphics, int size, const ResourceInfo &info) : 
		mGraphics(graphics), mSize(size), mResourceInfo(info) {}
public:
	virtual void update(void* data) = 0;

	auto getSize() -> int const {
		return mSize;
	}

	auto getInfo() -> ResourceInfo const {
		return mResourceInfo;
	}
};
