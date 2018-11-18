#pragma once

#include "ShaderBuffer.hpp"

class ConstantBuffer : public ShaderBuffer {
protected:
	ConstantBuffer(Graphics* graphics, int size) :
		ShaderBuffer(graphics, size) {}
};