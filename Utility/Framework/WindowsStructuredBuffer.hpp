#pragma once

#include "StructuredBuffer.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsGraphics;

class WindowsStructuredBuffer : public StructuredBuffer {
private:
	ID3D11Buffer* mBuffer;

	friend class WindowsGraphics;
public:
	WindowsStructuredBuffer(Graphics* graphics, int element_size, int element_count, const ResourceInfo& info);

	~WindowsStructuredBuffer();

	virtual void update(void* data)override;
};

#endif