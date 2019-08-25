#pragma once

#include "ShaderBuffer.hpp"

class VertexBuffer : public ShaderBuffer {
protected:
	int mStride;

	VertexBuffer(Graphics* graphics, int size, int stride, const ResourceInfo &info) :
		ShaderBuffer(graphics, size, info), mStride(stride) {}

public:
	auto getStride() -> int const {
		return mStride;
	}

	auto count() const -> int {
		return mSize / mStride;
	}
};