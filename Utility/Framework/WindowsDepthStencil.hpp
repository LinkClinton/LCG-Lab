#pragma once

#include "DepthStencil.hpp"

#ifdef _WIN32

#include <d3d11.h>
	
class Graphics;

class WindowsDepthStencil : public DepthStencil {
private:
	ID3D11DepthStencilView* mDepthStencil;

	friend class WindowsGraphics;
public:
	WindowsDepthStencil(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat);

	~WindowsDepthStencil();
};

#endif // _WIN32
