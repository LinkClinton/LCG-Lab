#pragma once

#include "VertexBuffer.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsVertexBuffer : public VertexBuffer {
private:
	ID3D11Buffer* mBuffer;

	friend class WindowsGraphics;
public:
	WindowsVertexBuffer(Graphics* graphics, int size, int stride);

	~WindowsVertexBuffer();

	virtual void update(void* data)override;
};

#endif // _WIN32

