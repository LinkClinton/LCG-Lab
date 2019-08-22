#pragma once

#include "ShaderBuffer.hpp"

class StructuredBuffer : public ShaderBuffer {
protected:
	int mElementSize;
	int mElementCount;

	StructuredBuffer(Graphics* graphics, int element_size, int element_count, const ResourceInfo& info) :
		ShaderBuffer(graphics, element_count* element_size, info), 
		mElementSize(element_size), mElementCount(element_count) {}
public:
	virtual auto element_size() -> int { return mElementSize; }

	virtual auto element_count() -> int { return mElementCount; }
};