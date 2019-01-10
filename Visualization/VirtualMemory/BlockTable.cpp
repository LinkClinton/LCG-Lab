#include "BlockTable.hpp"

Size BlockCache::mBlockCacheSize;

BlockCache::BlockCache(const Size & size, byte * data) : AddressMap(size)
{
	memcpy(getAddressPointer(), data, size.X * size.Y * size.Z);
}

BlockCache::BlockCache(const Size & size) : AddressMap(size)
{
	memset(getAddressPointer(), 0, size.X * size.Y * size.Z);
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
	//for block, we do not need to clear the block cache data
	//because the data will be covered with new data
}

BlockTable::BlockTable(const Size & size) : AddressMap(size),
	mMapRelation(size.X * size.Y * size.Z) 
{
	//compute the pool size and get address pointer
	auto memorySize = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//allocate memory(we do not change the size, so we can keep the address of vector)
	mMemoryPool.resize(memorySize);

	//get the address of virtual link
	for (int i = 0; i < memorySize; i++) arrayPointer[i] = &mMemoryPool[i];
}


BlockTable::~BlockTable()
{
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

	//get address and block size
	auto address = getAddress(virtualLink->Address);
	auto blockSize = BlockCache::getBlockCacheSize();
	
	//copy data
	memcpy(address->getAddressPointer(), blockCache->getAddressPointer(), blockSize.X * blockSize.Y * blockSize.Z);
}

auto BlockTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache *
{
	return getAddress(virtualLink->Address);
}
