#pragma once

#include "PageTable.hpp"

class PageDirectory : public AddressMap<VirtualLink*> {
private:
	PageTable* mNext;

	static auto allocateMemory(const std::vector<Size> &resolutionSize)->Size;
protected:
	std::vector<Size> mResolutionSize;
	std::vector<VirtualAddress> mResolutionEntry;
public:
	PageDirectory(const std::vector<Size> &resolutionSize, PageTable* nextTable);

	~PageDirectory();

	virtual void mapAddress(int resolution, const glm::vec3 &position, BlockCache* blockCache);

	virtual auto queryAddress(int resolution, const glm::vec3 &position) -> BlockCache*;
};
