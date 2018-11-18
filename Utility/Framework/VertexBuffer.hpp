#pragma once

#include "ShaderBuffer.hpp"

class VertexBuffer : public ShaderBuffer {
protected:
	int mStride;

	VertexBuffer(Graphics* graphics, int size, int stride) :
		ShaderBuffer(graphics, size), mStride(stride) {}

public:
	auto getStride() -> int const {
		return mStride;
	}
};