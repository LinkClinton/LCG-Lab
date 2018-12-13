#pragma once

#include <cassert>
#include <vector>

#include "AddressMap.hpp"
#include "BlockCache.hpp"

/*
 * @brief the table maps the virtual address from page to page(or from page to block cache)
 */
class PageTable : public AddressMap<VirtualAddress> {
private:
public:
	PageTable(Size size) : AddressMap(size) {}
};