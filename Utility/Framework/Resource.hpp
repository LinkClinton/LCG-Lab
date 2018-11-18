#pragma once

class Graphics;

class Resource {
protected:
	int mSize;

	Graphics* mGraphics;

	Resource(Graphics* graphics, int size) : 
		mGraphics(graphics), mSize(size) {}
public:
	virtual void update(void* data) = 0;

	auto getSize() -> int const {
		return mSize;
	}
};
