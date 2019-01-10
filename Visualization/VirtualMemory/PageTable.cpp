#include "PageTable.hpp"

Size PageCache::mPageCacheSize;

void PageTable::deletePageCache(PageCache *& pageCache)
{
	//we need to clear up the page cache data
	//and clear up the relation between virtual link in the page cache and the "mMapRelation" in the next table
	//if we do not clear up the relation, when we allocate an address who's "mMapRelation" is in the page cache need to clear up
	//the wrong virtual link was cleared, see "PageTable::clearUpAddress"
	//it does not effect the right of system but performance(we need map the data again, even if the data in the cache)
	pageCache->clearUp();
}

PageTable::PageTable(const Size &size, PageTable* nextTable) : AddressMap(size),
	mNext(nextTable), mEnd(nullptr), mMapRelation(size.X * size.Y * size.Z) 
{
	//compute the pool size and get address pointer
	auto memorySize = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//allocate memory(we do not change the size, so we can keep the address of vector)
	mMemoryPool.resize(memorySize);

	//get the address of virtual link
	for (int i = 0; i < memorySize; i++) arrayPointer[i] = &mMemoryPool[i];
}

PageTable::PageTable(const Size &size, BlockTable* endTable) : AddressMap(size),
	mNext(nullptr), mEnd(endTable), mMapRelation(size.X * size.Y * size.Z) 
{
	//compute the pool size and get address pointer
	auto memorySize = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//allocate memory(we do not change the size, so we can keep the address of vector)
	mMemoryPool.resize(memorySize);

	//get the address of virtual link
	for (int i = 0; i < memorySize; i++) arrayPointer[i] = &mMemoryPool[i];
}

PageTable::~PageTable()
{
}

void PageTable::mallocAddress(VirtualLink * virtualLink)
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

void PageTable::clearUpAddress(const VirtualAddress & address)
{
	//clear the relation between this page and last page at "address"
	//and clear the page cache at "address"
	//we do not need to clear the relation between this page and next page at "address"
	//because the mapped cache in this page at "address" will never be accessed
	//and when we need free virtual address in the next page, the LRU system will catch this "missed address"
	//Note: this operation do not tigger the LRU system

	//get array index
	auto arrayIndex = getArrayIndex(address);

	//clear the relation between this page and last page
	if (mMapRelation[arrayIndex] != nullptr) {
		mMapRelation[arrayIndex]->State = PageState::UnMapped;
		mMapRelation[arrayIndex] = nullptr;
	}

	//clear up the page cache
	deletePageCache(getAddressPointer()[arrayIndex]);
}

void PageTable::mapAddress(const glm::vec3 & position, const Size &size, BlockCache* blockCache, VirtualLink* virtualLink)
{
	//get page cache and page cache size
	auto pageCache = getAddress(virtualLink->Address);
	auto pageSize = pageCache->getSize();

	//compute the total size of current page level
	//compute the address from total size of current page level
	auto allSize = Helper::multiple(pageSize, size);
	auto address = Helper::multiple(allSize, position);

	//if position.xyz is one, the address will out of range
	//so we need to limit the address
	if (address.X == allSize.X) address.X = allSize.X - 1;
	if (address.Y == allSize.Y) address.Y = allSize.Y - 1;
	if (address.Z == allSize.Z) address.Z = allSize.Z - 1;

	//from address for virtual link
	//from address means the real address in the page table
	auto fromAddress = address;

	//we can get the relate address by using real address mod size
	address.X = address.X % pageSize.X;
	address.Y = address.Y % pageSize.Y;
	address.Z = address.Z % pageSize.Z;

	assert((mNext != nullptr) ^ (mEnd != nullptr));

	//get the address of next page
	auto nextAddress = pageCache->getAddress(address);

	//we use static memory allocation, so we do not set the address when memory are allocated
	//now set virtual link's from address to address
	nextAddress->FromAddress = fromAddress;

	//to next page, the end is null
	if (mNext != nullptr) {
		//if we do not map this page, we do it
		if (nextAddress->State == PageState::UnMapped) mNext->mallocAddress(nextAddress);

		//go to next layer
		mNext->mapAddress(position, allSize, blockCache, nextAddress);
	}

	//to block table, the next is null
	if (mEnd != nullptr) {
		if (nextAddress->State == PageState::UnMapped) mEnd->mallocAddress(nextAddress);

		mEnd->mapAddress(position, allSize, blockCache, nextAddress);
	}
}

auto PageTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache *
{
	//get page cache and size
	auto pageCache = getAddress(virtualLink->Address);
	auto pageSize = pageCache->getSize();

	//compute the total size of current page level
	//compute the address from total size of current page level
	auto allSize = Helper::multiple(pageSize, size);
	auto address = Helper::multiple(allSize, position);

	//if position.xyz is one, the address will out of range
	//so we need to limit the address
	if (address.X == allSize.X) address.X = allSize.X - 1;
	if (address.Y == allSize.Y) address.Y = allSize.Y - 1;
	if (address.Z == allSize.Z) address.Z = allSize.Z - 1;

	//we can get the relate address by using real address mod size
	address.X = address.X % pageSize.X;
	address.Y = address.Y % pageSize.Y;
	address.Z = address.Z % pageSize.Z;

	assert((mNext != nullptr) ^ (mEnd != nullptr));

	//get the address of next page
	auto nextAddress = pageCache->getAddress(address);

	//unmapped, so we only return null
	//about empty ? no-solution in this current version
	if (nextAddress->State != PageState::Mapped) return nullptr;

	//go to next layer to query address
	if (mNext != nullptr) return mNext->queryAddress(position, allSize, nextAddress);
	if (mEnd != nullptr) return mEnd->queryAddress(position, allSize, nextAddress);

	//error 
	assert(false); return nullptr;
}

PageCache::PageCache(const Size & size) : AddressMap(size)
{
	//compute the pool size and get address pointer
	auto memorySize = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//allocate memory(we do not change the size, so we can keep the address of vector)
	mMemoryPool.resize(memorySize);

	//get the address of virtual link
	for (int i = 0; i < memorySize; i++) arrayPointer[i] = &mMemoryPool[i];
}

void PageCache::clearUp()
{
	memset(&mMemoryPool[0], 0, sizeof(VirtualLink) * mMemoryPool.size());
}

void PageCache::setPageCacheSize(const Size & size)
{
	mPageCacheSize = size;
}

auto PageCache::getPageCacheSize() -> Size
{
	return mPageCacheSize;
}
