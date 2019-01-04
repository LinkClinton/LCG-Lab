#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>

#include "AddressMap.hpp"

class BlockCache : public AddressMap<byte> {
private:
	static Size mBlockCacheSize;
	VirtualAddress mOriginEntry; //for debug
public:
	BlockCache(const Size &size, const VirtualAddress &OriginEntry, byte* data);

	static void setBlockCacheSize(const Size &size);

	static auto getBlockCacheSize() -> Size;
};

class BlockTable : public AddressMap<BlockCache*> {
private:
	static void deleteBlockCache(BlockCache* &blockCache);
protected:
	std::vector<VirtualLink*> mMapRelation;
public:
	BlockTable(const Size &size) : AddressMap(size),
		mMapRelation(size.X * size.Y * size.Z) {}

	~BlockTable();

	virtual void mallocAddress(VirtualLink* virtualLink);

	virtual void clearUpAddress(const VirtualAddress &address);

	virtual void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink);

	virtual auto queryAddress(const glm::vec3 &position, const Size& size, VirtualLink* virtualLink) -> BlockCache*;
};
