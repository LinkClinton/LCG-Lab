#pragma once

#include "PageDirectory.hpp"
#include "GPUPageTable.hpp"

#include <Framework.hpp>

class GPUPageDirectory : public PageDirectory {
private:
	Graphics* mGraphics;
	Factory* mFactory;

	Texture3D* mPageDirectoryTexture;
public:
	GPUPageDirectory(Factory* factory, Graphics* graphics, const std::vector<Size> &resolutionSize, GPUPageTable* nextTable);

	~GPUPageDirectory();

	virtual void mapAddress(int resolution, const glm::vec3 &position, BlockCache* blockCache);

	virtual auto queryAddress(int resolution, const glm::vec3 &position)->BlockCache*;
};