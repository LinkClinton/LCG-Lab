#pragma once

#include "Resource.hpp"

class ShaderBuffer : public Resource{
protected:
	ShaderBuffer(Graphics* graphics, int size, const ResourceInfo &info) :
		Resource(graphics, size, info) {}
};