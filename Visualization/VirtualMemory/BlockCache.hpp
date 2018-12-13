#pragma once

#include "AddressMap.hpp"

/*
 * @brief the block who stored the real data.
 */
class BlockCache : public AddressMap<byte> {
public:
	BlockCache(Size size) : AddressMap(size) {}
};


