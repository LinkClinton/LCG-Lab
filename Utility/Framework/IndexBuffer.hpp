#pragma once

#include "ShaderBuffer.hpp"

class IndexBuffer : public ShaderBuffer {
protected:
	IndexBuffer(Graphics* graphics, int size, HeapType heapType) :
		ShaderBuffer(graphics, size, heapType) {}
};