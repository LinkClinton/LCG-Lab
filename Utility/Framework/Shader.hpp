#pragma once

#include <vector>

#include "Utility.hpp"

class Shader {
protected:
	std::vector<codebyte> mCodeBytes;
	std::vector<codebyte> mCompiledCodeBytes;

	Shader(const std::vector<codebyte> &codeBytes, bool isCompiled = false):
		mCodeBytes(codeBytes) {}
public:
	auto getCodeBytes() -> std::vector<codebyte> const {
		return mCodeBytes;
	}

	auto getCompiledCodeBytes() -> std::vector<codebyte> const {
		return mCompiledCodeBytes;
	}
};