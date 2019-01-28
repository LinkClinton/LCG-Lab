#pragma once

#include "ConstantBuffer.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsGraphics;

class WindowsConstantBuffer : public ConstantBuffer {
private:
	ID3D11Buffer* mBuffer;

	friend class WindowsGraphics;
public:
	WindowsConstantBuffer(Graphics* graphics, int size, const ResourceInfo &info);

	~WindowsConstantBuffer();

	virtual void update(void* data)override;
};

#endif // _WIN32
