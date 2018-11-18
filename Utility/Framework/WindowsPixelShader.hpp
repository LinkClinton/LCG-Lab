#pragma once

#include "PixelShader.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsPixelShader : public PixelShader {
private:
	ID3D11PixelShader* mPixelShader;

	friend class WindowsGraphics;
public:
	WindowsPixelShader(Graphics* graphics, const std::vector<codebyte> &codeBytes, bool isCompiled = false);

	~WindowsPixelShader();
};

#endif // _WIN32
