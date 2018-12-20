#pragma once

#include <cassert>
#include <vector>

#include <glm\glm.hpp>

#include "AddressMap.hpp"

class BlockCache : public AddressMap<byte> {
private:
	static Size mBlockCacheSize;
public:
	BlockCache(const Size &size = Size(0, 0, 0)) : AddressMap(size) {}

	BlockCache(const Size &size, byte* data);

	static void setBlockCacheSize(const Size &size);

	static auto getBlockCacheSize() -> Size;
};

class BlockTable : public AddressMap<BlockCache*> {
private:
	std::vector<VirtualLink*> mMapRelation;

	static void deleteBlockCache(BlockCache* &blockCache);
public:
	BlockTable(const Size &size) : AddressMap(size),
		mMapRelation(size.X * size.Y * size.Z) {}

	void mallocAddress(VirtualLink* virtualLink);

	void clearUpAddress(const VirtualAddress &address);

	void mapAddress(const glm::vec3 &position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink);

	auto queryAddress(const glm::vec3 &position, const Size& size, VirtualLink* virtualLink) -> BlockCache*;
};


