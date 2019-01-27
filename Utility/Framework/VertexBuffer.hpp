#pragma once

#include "ShaderBuffer.hpp"

class VertexBuffer : public ShaderBuffer {
protected:
	int mStride;

	VertexBuffer(Graphics* graphics, int size, int stride, HeapType heapType) :
		ShaderBuffer(graphics, size, heapType), mStride(stride) {}

public:
	auto getStride() -> int const {
		return mStride;
	}
};