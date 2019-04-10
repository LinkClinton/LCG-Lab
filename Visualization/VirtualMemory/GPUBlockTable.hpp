#pragma once

#include "BlockTable.hpp"

#include <Framework.hpp>

class GPUHelper {
public:
	static void modifyVirtualLinkToTexture(VirtualLink* virtualLink, Texture3D* texture);
};

class GPUBlockTable : public BlockTable {
private:
	Graphics* mGraphics;
	Factory* mFactory;

	Texture3D* mBlockTableTexture;
	Texture3D* mFromTexture;

	ResourceUsage* mTextureUsage;

	friend class GPUPageTable;
public:
	GPUBlockTable(Factory* factory, Graphics* graphics, const Size &size);

	~GPUBlockTable();

	void mallocAddress(VirtualLink* virtualLink)override;

	void clearUpAddress(const VirtualAddress &address)override;

	void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink)override;

	auto queryAddress(const glm::vec3 &position, const Size& size, VirtualLink* virtualLink)->BlockCache* override;

	auto getTexture() const -> Texture3D*;

	auto getTextureUsage() const -> ResourceUsage*;
};