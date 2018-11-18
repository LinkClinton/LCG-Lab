#pragma once

#include "Shader.hpp"

class PixelShader : public Shader {
protected:
	PixelShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false): 
		Shader(codeBytes, isCompiled) {}
};