#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

#include "AddressMap.hpp"
#include "BlockTable.hpp"

class PageCache : public AddressMap<VirtualLink*> {
private:
	static Size mPageCacheSize;
public:
	PageCache(const Size &size) : AddressMap(size) {}

	PageCache() : PageCache(mPageCacheSize) {}

	static void setPageCacheSize(const Size& size);

	static auto getPageCacheSize() -> Size;
};

class PageTable : public AddressMap<PageCache*> {
private:
	PageTable* mNext;
	BlockTable* mEnd;

	static void deletePageCache(PageCache* &pageCache);
protected:
	std::vector<VirtualLink*> mMapRelation;
public:
	PageTable(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable), mEnd(nullptr), mMapRelation(size.X * size.Y * size.Z) {}

	PageTable(const Size &size, BlockTable* endTable) : AddressMap(size),
		mNext(nullptr), mEnd(endTable), mMapRelation(size.X * size.Y * size.Z) {}

	~PageTable();

	virtual void mallocAddress(VirtualLink* virtualLink);

	virtual void clearUpAddress(const VirtualAddress &address);

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink);

	virtual auto queryAddress(const glm::vec3 &position, const Size & size, VirtualLink* virtualLink) -> BlockCache*;
};

class PageDirectory : public AddressMap<VirtualLink*> {
private:
	PageTable* mNext;
public:
	PageDirectory(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable) {}

	~PageDirectory();

	void mapAddress(const glm::vec3 &position, BlockCache* blockCache);

	auto queryAddress(const glm::vec3 &position) -> BlockCache*;
};

