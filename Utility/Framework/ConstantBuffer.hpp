#pragma once

#include "ShaderBuffer.hpp"

class ConstantBuffer : public ShaderBuffer {
protected:
	ConstantBuffer(Graphics* graphics, int size, const ResourceInfo &info) :
		ShaderBuffer(graphics, size, info) {}
};