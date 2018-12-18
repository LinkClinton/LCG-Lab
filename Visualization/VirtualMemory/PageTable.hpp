#pragma once

#include <cassert>
#include <vector>

#include <glm\glm.hpp>

#include "AddressMap.hpp"
#include "BlockTable.hpp"

enum class PageState {
	Mapped = 0,
	UnMapped = 1,
	Empty = 2
};

struct VirtualLink {
	VirtualAddress Address;
	PageState State;

	VirtualLink(const VirtualAddress &address = VirtualAddress(),
		PageState state = PageState::UnMapped) : Address(address), State(state) {}
};

class PageCache : public AddressMap<VirtualLink*> {
public:
	PageCache(const Size &size = Size(0, 0, 0)) : AddressMap(size) {}
};

class PageTable : public AddressMap<PageCache*> {
private:
	PageTable* mNext;
	BlockTable* mEnd;

	std::vector<VirtualLink*> mMapRelation;
public:
	PageTable(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable), mEnd(nullptr), mMapRelation(size.X * size.Y * size.Z) {}

	PageTable(const Size &size, BlockTable* endTable) : AddressMap(size),
		mNext(nullptr), mEnd(endTable), mMapRelation(size.X * size.Y * size.Z) {}

	void mallocAddress(VirtualLink* virtualLink);

	void clearUpAddress(const VirtualAddress &address);

	void mapAddress(const glm::vec3 &position,  BlockCache* blockCache, VirtualLink* virtualLink);
};

class PageDirectory : public AddressMap<VirtualLink*> {
private:
	PageTable* mNext;
public:
	PageDirectory(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable) {}

	void mapAddress(const glm::vec3 &position, BlockCache* blockCache);
};

