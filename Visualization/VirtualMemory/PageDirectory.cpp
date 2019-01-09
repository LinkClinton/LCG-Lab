#include "PageDirectory.hpp"

auto PageDirectory::allocateMemory(const std::vector<Size>& resolutionSize) -> Size
{
	//compute the size of page directory(support multi-resolution)
	Size size;

	//because we need to store the directory for multi-resolution
	//so we need store all directories
	//the y and z(height and depth) size are the biggest size of directory(resolution 0)
	//the x(width) size is the sum of directory's x size
	assert(resolutionSize.empty() == false);

	//init the size
	size.X = resolutionSize[0].X;
	size.Y = resolutionSize[0].Y;
	size.Z = resolutionSize[0].Z;

	//get the x size
	for (size_t i = 1; i < resolutionSize.size(); i++)
		size.X = size.X + resolutionSize[i].X;

	return size;
}

PageDirectory::PageDirectory(const std::vector<Size>& resolutionSize, PageTable * nextTable)
	: AddressMap(allocateMemory(resolutionSize)), mResolutionSize(resolutionSize), mNext(nextTable)
{
	assert(mNext != nullptr);

	mResolutionEntry.resize(mResolutionSize.size());

	int xLocation = 0;

	for (size_t i = 0; i < mResolutionEntry.size(); i++) {
		//x-Location will be added with the x-size of multi-Size
		//y-Location and z-Location are always 0 
		mResolutionEntry[i] = VirtualAddress(xLocation, 0, 0);

		xLocation = xLocation + mResolutionSize[i].X;
	}
}

PageDirectory::~PageDirectory()
{
	size_t size = mSize.X * mSize.Y * mSize.Z;
	auto arrayPointer = getAddressPointer();

	for (size_t i = 0; i < size; i++) {
		if (arrayPointer[i] == nullptr) continue;

		delete arrayPointer[i]; arrayPointer[i] = nullptr;
	}
}

void PageDirectory::mapAddress(int resolution, const glm::vec3 & position, BlockCache * blockCache)
{
	assert((size_t)resolution < mResolutionEntry.size());

	//compute the address
	auto resolutionSize = mResolutionSize[resolution];
	auto address = Helper::multiple(resolutionSize, position);

	//if position.xyz is one, the address will be out of range
	//so we need to limit the address
	if (address.X == resolutionSize.X) address.X = resolutionSize.X - 1;
	if (address.Y == resolutionSize.Y) address.Y = resolutionSize.Y - 1;
	if (address.Z == resolutionSize.Z) address.Z = resolutionSize.Z - 1;

	//get the real address in the page directory
	address = Helper::add(mResolutionEntry[resolution], address);
	
	//get the address of next page
	auto nextAddress = getAddress(address);

	//if address is null, we create and set it
	if (nextAddress == nullptr) setAddress(address, nextAddress = new VirtualLink(VirtualAddress(), address, PageState::UnMapped));

	//if address is null, we create and set it
	if (nextAddress->State == PageState::UnMapped) mNext->mallocAddress(nextAddress);

	//go to next layer
	mNext->mapAddress(position, resolutionSize, blockCache, nextAddress);
}

auto PageDirectory::queryAddress(int resolution, const glm::vec3 & position) -> BlockCache *
{
	assert((size_t)resolution < mResolutionEntry.size());

	//compute the address
	auto resolutionSize = mResolutionSize[resolution];
	auto address = Helper::multiple(resolutionSize, position);

	//if position.xyz is one, the address will be out of range
	//so we need to limit the address
	if (address.X == resolutionSize.X) address.X = resolutionSize.X - 1;
	if (address.Y == resolutionSize.Y) address.Y = resolutionSize.Y - 1;
	if (address.Z == resolutionSize.Z) address.Z = resolutionSize.Z - 1;

	//get the real address in the page directory
	address = Helper::add(mResolutionEntry[resolution], address);
	
	//get the address of next page
	auto nextAddress = getAddress(address);

	//unmapped, so we only return null
	//about empty ? no-solution in this current version
	if (nextAddress == nullptr || nextAddress->State != PageState::Mapped) return nullptr;

	//go to next layer and query address
	return mNext->queryAddress(position, mSize, nextAddress);
}
