#pragma once

#include "AddressMap.hpp"

class BlockCache : public AddressMap<byte> {
public:
	BlockCache(const Size &size = Size(0, 0, 0)) : AddressMap(size) {}

	BlockCache(const Size &size, byte* data);
};

class BlockTable : public AddressMap<BlockCache*> {
public:
	BlockTable(const Size &size) : AddressMap(size) {}
};


