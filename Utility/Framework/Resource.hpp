#pragma once

class Graphics;

#include "HeapType.hpp"

class Resource {
protected:
	int mSize;

	HeapType mHeapType;

	Graphics* mGraphics;

	Resource(Graphics* graphics, int size, HeapType heapType) : 
		mGraphics(graphics), mSize(size), mHeapType(heapType) {}
public:
	virtual void update(void* data) = 0;

	auto getSize() -> int const {
		return mSize;
	}

	auto getHeapType() -> HeapType const {
		return mHeapType;
	}
};
