#pragma once

#include "UnorderedAccessUsage.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsUnorderedAccessUsage : public UnorderedAccessUsage {
private:
	ID3D11UnorderedAccessView* mUnorderedAccessView;

	friend class WindowsGraphics;
public:
	WindowsUnorderedAccessUsage(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat);

	WindowsUnorderedAccessUsage(Graphics* graphics, Texture3D* texture3D, PixelFormat pixelFormat);

	~WindowsUnorderedAccessUsage();
};


#endif // _WIN32