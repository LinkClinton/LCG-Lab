#pragma once

#include "Utility.hpp"
#include "LRUCache.hpp"

#include <cassert>
#include <vector>

template<typename T>
class AddressMap {
private:
	Size mSize;
	std::vector<T> mAddress;

	LRUCache<VirtualAddress, VirtualAddress::HashFunction> mLRUCache;

	int mRowPitch;
	int mDepthPitch;
public:
	AddressMap(const Size &size = Size(0, 0, 0)) {
		mSize = size;
		mAddress.resize(mSize.X * mSize.Y * mSize.Z);

		//set LRUCache size
		for (int x = 0; x < mSize.X; x++)
			for (int y = 0; y < mSize.Y; y++)
				for (int z = 0; z < mSize.Z; z++)
					mLRUCache.addElement(VirtualAddress(x, y, z));

		mRowPitch = mSize.Y;
		mDepthPitch = mSize.X * mSize.Y;
	}

	auto getSize() -> Size {
		return mSize;
	}

	auto getAddressPointer() -> T* const {
		return &mAddress[0];
	}

	void setAddress(const VirtualAddress &index, const T &address) {
		//access index for LRU
		mLRUCache.accessElement(index);

		//set value
		mAddress[getArrayIndex(index)] = address;
	}

	auto getAddress(const VirtualAddress &index) -> T {
		//access index for LRU
		mLRUCache.accessElement(index);

		//return value
		return mAddress[getArrayIndex(index)];
	}

	auto mallocAddress() -> VirtualAddress {
		//malloc an address to user
		//if we have empty memory, we will return its address
		//else we will return the address of LRUCache's back element
		//and the back element of LRUCache's will be accessed
		
		//get address
		auto address = mLRUCache.getBackElement();

		//access the back element
		mLRUCache.accessElement(address);

		return address;
	}

	auto getArrayIndex(const VirtualAddress &index) -> int {
		assert(index.X >= 0 && index.Y >= 0 && index.Z >= 0);
		assert(index.X < mSize.X && index.Y < mSize.Y && index.Z < mSize.Z);

		return index.Z * mDepthPitch + index.Y * mRowPitch + index.X;
	}
};