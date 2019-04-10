#pragma once

#include "PageDirectory.hpp"
#include "GPUPageTable.hpp"

#include <Framework.hpp>

class GPUPageDirectory : public PageDirectory {
private:
	Graphics* mGraphics;
	Factory* mFactory;

	Texture3D* mPageDirectoryTexture;

	ResourceUsage* mTextureUsage;
public:
	GPUPageDirectory(Factory* factory, Graphics* graphics, const std::vector<Size> &resolutionSize, GPUPageTable* nextTable);

	~GPUPageDirectory();

	void mapAddress(int resolution, const glm::vec3 &position, BlockCache* blockCache)override;

	auto queryAddress(int resolution, const glm::vec3 &position)->BlockCache* override;

	auto getTexture() const -> Texture3D*;

	auto getTextureUsage() const -> ResourceUsage*;
};