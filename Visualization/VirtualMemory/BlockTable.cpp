#include "BlockTable.hpp"
#include "PageTable.hpp"

Size BlockCache::mBlockCacheSize;

BlockCache::BlockCache(const Size & size, byte * data) : DataCache(size)
{
	memcpy(getDataPointer(), data, size.X * size.Y * size.Z);
}

BlockCache::BlockCache(const Size & size) : DataCache(size)
{
	memset(getDataPointer(), 0, size.X * size.Y * size.Z);
}

auto BlockCache::average(const VirtualAddress& from, const VirtualAddress& to) -> byte
{
	unsigned long long value = 0;

	for (size_t z = from.Z; z < to.Z; z++) {
		for (size_t y = from.Y; y < to.Y; y++) {
			const auto baseAddress = z * mDepthPitch + y * mRowPitch;
			for (size_t address = baseAddress + from.X; address < baseAddress + to.X; address++)
				value = value + mData[address];
		}
	}

	const auto size = Size(to.X - from.X, to.Y - from.Y, to.Z - from.Z);

	return byte(value / (size.X * size.Y * size.Z));
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
	mFromTable(nullptr), mMapRelation(size.X * size.Y * size.Z)
{
	//compute the pool size and get address pointer
	const auto memorySize = mSize.X * mSize.Y * mSize.Z;
	const auto arrayPointer = getAddressPointer();

	//allocate memory(we do not change the size, so we can keep the address of vector)
	mMemoryPool.resize(memorySize);

	//get the address of virtual link
	for (auto i = 0; i < memorySize; i++) arrayPointer[i] = &mMemoryPool[i];
}


BlockTable::~BlockTable()
{
}

void BlockTable::mallocAddress(VirtualLink * virtualLink)
{
	assert(virtualLink != nullptr);

	//malloc address and get array index
	const auto address = AddressMap::mallocAddress();
	const auto arrayIndex = AddressMap::getArrayIndex(address);

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
	const auto arrayIndex = getArrayIndex(address);

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
	const auto address = getAddress(virtualLink->Address);
	const auto blockSize = BlockCache::getBlockCacheSize();
	
	//copy data
	memcpy(address->getDataPointer(), blockCache->getDataPointer(), blockSize.X * blockSize.Y * blockSize.Z);
}

auto BlockTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache *
{
	return getAddress(virtualLink->Address);
}

auto BlockTable::invertQuery(const VirtualAddress &address) -> PageDirectory* {
	//trigger the LRU system
	getAddress(address);

	return mFromTable->invertQuery(mMapRelation[getArrayIndex(address)]);
}