#pragma once

#include "Texture3D.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsTexture3D : public Texture3D {
private:
	ID3D11Texture3D* mTexture3D;
	
	friend class WindowsGraphics;
	friend class WindowsResourceUsage;
	friend class WindowsUnorderedAccessUsage;
public:
	WindowsTexture3D(Graphics* graphics, int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info);

	~WindowsTexture3D();

	virtual void update(void* data)override;

	virtual void update(void* data, int left, int top, int front, int right, int bottom, int back)override;

	virtual void copy(Texture3D* source)override;

	virtual auto map() -> MappedData override;

	virtual void unmap()override;
};

#endif // _WIN32
