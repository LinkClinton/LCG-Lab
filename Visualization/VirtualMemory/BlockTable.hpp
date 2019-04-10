#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "DataCache.hpp"
#include "AddressMap.hpp"

class PageTable;
class PageDirectory;

class BlockCache : public DataCache<byte> {
private:
	static Size mBlockCacheSize;
public:
	BlockCache(const Size &size, byte* data);

	BlockCache(const Size &size);

	BlockCache() : BlockCache(mBlockCacheSize) {}

	auto average(const VirtualAddress &from, const VirtualAddress &to) -> byte;

	static void setBlockCacheSize(const Size &size);

	static auto getBlockCacheSize() -> Size;
};

class BlockTable : public AddressMap<BlockCache*> {
private:
	std::vector<BlockCache> mMemoryPool;

	PageTable* mFromTable;

	static void deleteBlockCache(BlockCache* &blockCache);

	friend class PageTable;
protected:
	std::vector<VirtualLink*> mMapRelation;
public:
	BlockTable(const Size &size);

	~BlockTable();

	virtual void mallocAddress(VirtualLink* virtualLink);

	virtual void clearUpAddress(const VirtualAddress &address);

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink);

	virtual auto queryAddress(const glm::vec3 &position, const Size& size, VirtualLink* virtualLink) -> BlockCache*;

	virtual auto invertQuery(const VirtualAddress &address) -> PageDirectory*;
};
