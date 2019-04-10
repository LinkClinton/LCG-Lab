#pragma once

#include "Helper.hpp"

#include <vector>
#include <cassert>

template<typename T>
class DataCache {
protected:
	std::vector<T> mData;

	int mRowPitch;
	int mDepthPitch;

	Size mSize;
public:
	DataCache(const Size& size = Size(0, 0, 0)) {
		mSize = size;
		mData.resize(mSize.X * mSize.Y * mSize.Z);

		mRowPitch = mSize.X;
		mDepthPitch = mSize.X * mSize.Y;	
	}

	virtual ~DataCache() = default;

	auto getSize() const -> Size {
		return mSize;
	}

	auto getDataPointer() -> T* const {
		return &mData[0];
	}

	void virtual setAddress(const VirtualAddress &index, const T &address) {
		//set value
		mData[getArrayIndex(index)] = address;
	}

	auto virtual getAddress(const VirtualAddress &index) -> T {
		//return value
		return mData[getArrayIndex(index)];
	}

	auto getArrayIndex(const VirtualAddress &index) const -> int {
		assert(index.X >= 0 && index.Y >= 0 && index.Z >= 0);
		assert(index.X < mSize.X && index.Y < mSize.Y && index.Z < mSize.Z);

		return index.Z * mDepthPitch + index.Y * mRowPitch + index.X;
	}
};