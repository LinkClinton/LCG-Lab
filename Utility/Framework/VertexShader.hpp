#pragma once

#include "Shader.hpp"

class VertexShader : public Shader{
protected:
	VertexShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) :
		Shader(codeBytes, isCompiled) {}
};