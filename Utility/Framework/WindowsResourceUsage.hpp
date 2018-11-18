#pragma once

#include "ResourceUsage.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsResourceUsage : public ResourceUsage {
private:
	ID3D11ShaderResourceView* mShaderResourceView;

	friend class WindowsGraphics;
public:
	WindowsResourceUsage(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat);

	WindowsResourceUsage(Graphics* graphics, Texture3D* texture3D, PixelFormat pixelFormat);

	~WindowsResourceUsage();
};


#endif // _WIN32