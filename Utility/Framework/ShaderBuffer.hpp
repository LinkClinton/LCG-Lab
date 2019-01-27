#pragma once

#include "Resource.hpp"

class ShaderBuffer : public Resource{
protected:
	ShaderBuffer(Graphics* graphics, int size, HeapType heapType) : 
		Resource(graphics, size, heapType) {}
};