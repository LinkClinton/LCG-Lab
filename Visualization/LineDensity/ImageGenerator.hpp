#pragma once

#include "Utility.hpp"

#include <DirectXTex.h>

class DensityGenerator;

class ImageGenerator {
public:
	static void save(const std::wstring& fileName, const DensityGenerator& densityGenerator);
private:
};
