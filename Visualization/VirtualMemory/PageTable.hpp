#pragma once

#include <cassert>
#include <vector>

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
		PageState state = PageState::Empty) : Address(address), State(state) {}
};

class PageCache : public AddressMap<VirtualLink> {
public:
	PageCache(const Size &size = Size(0, 0, 0)) : AddressMap(size) {}
};

class PageTable : public AddressMap<PageCache> {
private:
	PageTable* mNext;
	BlockTable* mEnd;
public:
	PageTable(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable), mEnd(nullptr) {}

	PageTable(const Size &size, BlockTable* endTable) : AddressMap(size),
		mNext(nullptr), mEnd(endTable) {}
};

class PageDirectory : public AddressMap<VirtualLink> {
private:
	PageTable* mNext;
public:
	PageDirectory(const Size &size, PageTable* nextTable) : AddressMap(size),
		mNext(nextTable) {}


};

