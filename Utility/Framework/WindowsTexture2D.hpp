#pragma once

#include "Texture2D.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsTexture2D : public Texture2D {
private:
	ID3D11Texture2D* mTexture2D;
	
	friend class WindowsGraphics;
	friend class WindowsRenderTarget;
	friend class WindowsDepthStencil;
	friend class WindowsResourceUsage;
	friend class WindowsUnorderedAccessUsage;
public:
	WindowsTexture2D(Graphics* graphics, int width, int height, PixelFormat pixelFormat, const ResourceInfo &info);

	~WindowsTexture2D();

	virtual void update(void* data)override;

	virtual auto map()->MappedData override;

	virtual void unmap()override;

	virtual void save(const std::wstring& fileName)override;
};

#endif // _WIN32

