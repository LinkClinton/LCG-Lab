#pragma once

#include "ShaderBuffer.hpp"

class IndexBuffer : public ShaderBuffer {
protected:
	IndexBuffer(Graphics* graphics, int size) :
		ShaderBuffer(graphics, size) {}
};