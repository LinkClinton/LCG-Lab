#pragma once

#include "BlockTable.hpp"

#include <Framework.hpp>

class GPUBlockTable : public BlockTable {
private:
	Graphics* mGraphics;
	Factory* mFactory;

	Texture3D* mBlockTableTexture;
public:
	GPUBlockTable(Factory* factory, Graphics* graphics, const Size &size);

	~GPUBlockTable();

	virtual void mallocAddress(VirtualLink* virtualLink)override;

	virtual void clearUpAddress(const VirtualAddress &address)override;

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink)override;

	virtual auto queryAddress(const glm::vec3 &position, const Size& size, VirtualLink* virtualLink)->BlockCache* override;
};