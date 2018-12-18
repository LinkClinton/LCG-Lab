#include "BlockTable.hpp"

BlockCache::BlockCache(const Size & size, byte * data) : AddressMap(size)
{
	memcpy(getAddressPointer(), data, size.X * size.Y * size.Z);
}
