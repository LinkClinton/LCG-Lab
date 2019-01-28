#pragma once

#include "ShaderBuffer.hpp"

class IndexBuffer : public ShaderBuffer {
protected:
	IndexBuffer(Graphics* graphics, int size, const ResourceInfo &info) :
		ShaderBuffer(graphics, size, info) {}
};