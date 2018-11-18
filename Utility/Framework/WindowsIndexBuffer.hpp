#pragma once

#include "IndexBuffer.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsGraphics;

class WindowsIndexBuffer : public IndexBuffer {
private:
	ID3D11Buffer* mBuffer;

	friend class WindowsGraphics;
public:
	WindowsIndexBuffer(Graphics* graphics, int size);

	~WindowsIndexBuffer();

	virtual void update(void* data)override;
};

#endif // _WIN32
