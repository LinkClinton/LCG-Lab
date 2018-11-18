#pragma once

#include "VertexShader.hpp"

#ifdef _WIN32

#include <d3d11.h>

class Graphics;

class WindowsVertexShader : public VertexShader {
private:
	ID3D11VertexShader* mVertexShader;

	friend class WindowsGraphics;
public:
	WindowsVertexShader(Graphics* graphics, const std::vector<codebyte> &codeBytes, bool isCompiled = false);

	~WindowsVertexShader();
};

#endif // _WIN32
