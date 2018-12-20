#include "BlockTable.hpp"

Size BlockCache::mBlockCacheSize;

BlockCache::BlockCache(const Size & size, const VirtualAddress &originEntry, byte * data) : AddressMap(size)
{
	mOriginEntry = originEntry;
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

	if (blockCache == nullptr) return;

	delete blockCache; blockCache = nullptr;
}

BlockTable::~BlockTable()
{
	//get size and array
	size_t size = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//delete all block cache
	for (size_t i = 0; i < size; i++) {
		if (arrayPointer[i] == nullptr) continue;

		delete arrayPointer[i]; arrayPointer[i] = nullptr;
	}
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

auto BlockTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache *
{
	return getAddress(virtualLink->Address);
}
