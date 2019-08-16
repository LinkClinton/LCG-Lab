#include "ImageGenerator.hpp"
#include "DensityGenerator.hpp"

void ImageGenerator::save(
	const std::wstring &fileName,
	const DensityGenerator& densityGenerator) {

	densityGenerator.mHeatMap->save(fileName);
}

