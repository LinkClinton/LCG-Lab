#pragma once

#include "Utility.hpp"

#include <cassert>
#include <vector>

template<typename T>
class AddressMap {
private:
	Size mSize;
	std::vector<T> mAddress;

	int mRowPitch;
	int mDepthPitch;
public:
	AddressMap(Size size) {
		mSize = size;
		mAddress.resize(mSize.X * mSize.Y * mSize.Z);

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
		mAddress[getArrayIndex(index)] = address;
	}

	auto getAddress(const VirtualAddress &index) -> T {
		return mAddress[getArrayIndex(index)];
	}

	auto getArrayIndex(const VirtualAddress &index) -> int {
		assert(index.X >= 0 && index.Y >= 0 && index.Z >= 0);
		assert(index.X < mSize.X && index.Y < mSize.Y && index.Z < mSize.Z);

		return index.Z * mDepthPitch + index.Y * mRowPitch + index.X;
	}
};