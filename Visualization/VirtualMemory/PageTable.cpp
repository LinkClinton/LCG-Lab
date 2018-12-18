#include "PageTable.hpp"

void PageDirectory::mapAddress(const glm::vec3 & position, BlockCache* blockCache)
{
	assert(mSize.X != 0 && mSize.Y != 0 && mSize.Z != 0);

	//compute the address
	auto address = VirtualAddress(
		int(mSize.X * position.x),
		int(mSize.Y * position.y),
		int(mSize.Z * position.z));

	//if position.xyz is one, the address will out of range
	//so we need to limit the address
	if (address.X == mSize.X) address.X = mSize.X - 1;
	if (address.Y == mSize.Y) address.Y = mSize.Y - 1;
	if (address.Z == mSize.Z) address.Z = mSize.Z - 1;

	assert(mNext != nullptr);

	//get the address of next page
	auto nextAddress = getAddress(address);

	//if address is null, we create and set it.
	if (nextAddress == nullptr) setAddress(address, nextAddress = new VirtualLink());

	//if we do not map this page, we do it
	if (nextAddress->State == PageState::UnMapped) mNext->mallocAddress(nextAddress);

	//go to next layer
	mNext->mapAddress(position, blockCache, nextAddress);
}

void PageTable::mallocAddress(VirtualLink * virtualLink)
{
	assert(virtualLink != nullptr);

	//malloc address and get array index
	auto address = AddressMap::mallocAddress();
	auto arrayIndex = AddressMap::getArrayIndex(address);

	//clear the reference
	if (mMapRelation[arrayIndex] != nullptr) mMapRelation[arrayIndex]->State = PageState::UnMapped;

	//set new reference
	mMapRelation[arrayIndex] = virtualLink;

	//set the address to virtual link
	virtualLink->Address = address;
	virtualLink->State = PageState::Mapped;
}

void PageTable::clearUpAddress(const VirtualAddress & address)
{
	//get array index
	auto arrayIndex = getArrayIndex(address);
	auto pageCache = getAddressPointer();

	//clear the relation between this page and last page
	if (mMapRelation[arrayIndex] != nullptr) {
		mMapRelation[arrayIndex]->State = PageState::UnMapped;
		mMapRelation[arrayIndex] = nullptr;
	}

	
}

void PageTable::mapAddress(const glm::vec3 & position, BlockCache* blockCache, VirtualLink* virtualLink)
{
	
}
