#pragma once

#include "PageTable.hpp"
#include "GPUBlockTable.hpp"

#include <Framework.hpp>

class GPUPageTable : public PageTable {
private:
	Graphics* mGraphics;
	Factory* mFactory;

	Texture3D* mPageTableTexture;
	Texture3D* mFromTexture;

	friend class GPUPageDirectory;
public:
	GPUPageTable(Factory* factory, Graphics* graphics, const Size &size, GPUPageTable* nextTable);

	GPUPageTable(Factory* factory, Graphics* graphics, const Size &size, GPUBlockTable* endTable);

	~GPUPageTable();

	virtual void mallocAddress(VirtualLink* virtualLink)override;

	virtual void clearUpAddress(const VirtualAddress &address)override;

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink)override;

	virtual auto queryAddress(const glm::vec3 &position, const Size & size, VirtualLink* virtualLink)->BlockCache* override;
};