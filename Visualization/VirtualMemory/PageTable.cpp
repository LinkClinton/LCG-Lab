#include "PageTable.hpp"

Size PageCache::mPageCacheSize;

void PageTable::deletePageCache(PageCache *& pageCache)
{
	//delete the page cache, we have two different way
	//for current version, we will delete all memory and set it to null
	//another way, we will record a no-free array and set its no-free elments to null
	
	if (pageCache == nullptr) return;
	
	//delete 
	delete pageCache; pageCache = nullptr;
}

PageTable::~PageTable()
{
	//get size and array
	size_t size = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	//delete resource
	for (size_t i = 0; i < size; i++) {
		if (arrayPointer[i] == nullptr) continue;

		delete arrayPointer[i]; arrayPointer[i] = nullptr;
	}
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

	//if the cache is null, we create it with default size
	//default size: mPageCacheSize(static)
	if (getAddressPointer()[arrayIndex] == nullptr)
		getAddressPointer()[arrayIndex] = new PageCache();
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
	assert(mSize.X != 0 && mSize.Y != 0 && mSize.Z != 0);

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

	assert(pageCache != nullptr);
	assert((mNext != nullptr) ^ (mEnd != nullptr));

	//get the address of next page
	auto nextAddress = pageCache->getAddress(address);

	//if address is null, we create and set it
	if (nextAddress == nullptr) pageCache->setAddress(address, nextAddress = new VirtualLink(VirtualAddress(), fromAddress, PageState::UnMapped));

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
	assert(mSize.X != 0 && mSize.Y != 0 && mSize.Z != 0);

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

	assert(pageCache != nullptr);
	assert((mNext != nullptr) ^ (mEnd != nullptr));

	//get the address of next page
	auto nextAddress = pageCache->getAddress(address);

	//unmapped, so we only return null
	//about empty ? no-solution in this current version
	if (nextAddress == nullptr || nextAddress->State != PageState::Mapped) return nullptr;

	//go to next layer to query address
	if (mNext != nullptr) return mNext->queryAddress(position, allSize, nextAddress);
	if (mEnd != nullptr) return mEnd->queryAddress(position, allSize, nextAddress);

	//error 
	assert(false); return nullptr;
}

void PageCache::setPageCacheSize(const Size & size)
{
	mPageCacheSize = size;
}

auto PageCache::getPageCacheSize() -> Size
{
	return mPageCacheSize;
}
