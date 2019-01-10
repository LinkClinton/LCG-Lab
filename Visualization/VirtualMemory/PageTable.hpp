#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

#include "AddressMap.hpp"
#include "BlockTable.hpp"

class PageCache : public AddressMap<VirtualLink*> {
private:
	std::vector<VirtualLink> mMemoryPool;

	static Size mPageCacheSize;
public:
	PageCache(const Size &size);

	PageCache() : PageCache(mPageCacheSize) {}

	void clearUp();

	static void setPageCacheSize(const Size& size);

	static auto getPageCacheSize() -> Size;
};

class PageTable : public AddressMap<PageCache*> {
private:
	std::vector<PageCache> mMemoryPool;

	PageTable* mNext;
	BlockTable* mEnd;

	static void deletePageCache(PageCache* &pageCache);
protected:
	std::vector<VirtualLink*> mMapRelation;
public:
	PageTable(const Size &size, PageTable* nextTable);

	PageTable(const Size &size, BlockTable* endTable);

	~PageTable();

	virtual void mallocAddress(VirtualLink* virtualLink);

	virtual void clearUpAddress(const VirtualAddress &address);

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink);

	virtual auto queryAddress(const glm::vec3 &position, const Size & size, VirtualLink* virtualLink) -> BlockCache*;
};


