#include "BlockTable.hpp"

Size BlockCache::mBlockCacheSize = Size(0, 0, 0);

BlockCache::BlockCache(const Size & size, byte * data) : AddressMap(size)
{
	memcpy(getAddressPointer(), data, size.X * size.Y * size.Z);
}

void BlockCache::setBlockCacheSize(const Size & size)
{
	mBlockCacheSize = size;
}

auto BlockCache::getBlockCacheSize() -> Size
{
	return mBlockCacheSize;
}

void BlockTable::deleteBlockCache(BlockCache *& blockCache)
{
	//delete the block cache, we have two different way
	//for current version, we will delete all memory and set it to null
	//another way, we will record a no-free array and set its no-free elments to null

	assert(blockCache != nullptr);

	delete blockCache; blockCache = nullptr;
}

void BlockTable::mallocAddress(VirtualLink * virtualLink)
{
	assert(virtualLink != nullptr);

	//malloc address and get array index
	auto address = AddressMap::mallocAddress();
	auto arrayIndex = AddressMap::getArrayIndex(address);

	//clear up
	clearUpAddress(address);

	//set new reference relation
	mMapRelation[arrayIndex] = virtualLink;

	//set the address to virtual link
	virtualLink->Address = address;
	virtualLink->State = PageState::Mapped;
}

void BlockTable::clearUpAddress(const VirtualAddress & address)
{
	//clear the relation between this page and last page at "address"
	//and clear the block cache at "address"
	
	//get array index
	auto arrayIndex = getArrayIndex(address);

	//clear the relation between this page and last page
	if (mMapRelation[arrayIndex] != nullptr) {
		mMapRelation[arrayIndex]->State = PageState::UnMapped;
		mMapRelation[arrayIndex] = nullptr;
	}

	//clear up the page cache
	deleteBlockCache(getAddressPointer()[arrayIndex]);
}

void BlockTable::mapAddress(const glm::vec3 & position, const Size & size, BlockCache * blockCache, VirtualLink * virtualLink)
{
	//test 
	assert(blockCache != nullptr);
	assert(blockCache->getSize() == BlockCache::getBlockCacheSize());

	//set data
	setAddress(virtualLink->Address, blockCache);
}
