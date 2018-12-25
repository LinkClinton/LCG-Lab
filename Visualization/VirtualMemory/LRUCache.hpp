#pragma once

#include "Helper.hpp"

#include <list>
#include <unordered_map>
#include <cassert>

template<typename T, typename HashFunction>
class LRUCache {
private:
	std::list<T> mList;
	std::unordered_map<T, typename std::list<T>::iterator, HashFunction> mMap;
public:
	LRUCache() = default;

	void addElement(const T &element) {
		//assert map do not contain the element
		assert(mMap.count(element) == 0);
		
		//add element, we use unordered map to hash the element
		//the element must be the head of list
		mMap.insert(std::pair<T, typename std::list<T>::iterator>(element, mList.insert(mList.begin(), element)));
	}

	void removeElement(const T &element) {
		//assert map contains the element
		assert(mMap.count(element) != 0);

		//remove element
		mList.erase(mMap[element]);
		mMap.erase(element);
	}

	void accessElement(const T &element) {
		//access Element, means we put it to the head of list
		//if element is not existed, we will add it.

		//because the iterator will be lost efficacy after "erase"
		//we need "erase" it
		removeElement(element);

		//add elemen, put the element to head of list
		addElement(element);
	}

	auto getFrontElement() -> T {
		assert(mList.size() != 0);

		//get the head of list
		return *(mList.begin());
	}

	auto getBackElement() -> T {
		assert(mList.size() != 0);

		//get the tail of list
		return *(--mList.end());
	}
};