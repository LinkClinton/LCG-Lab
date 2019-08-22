#pragma once

#include "Utility.hpp"
#include "SharedMacro.hpp"


class ColorMapped {
public:
	ColorMapped() : mSpace(0) {}

	ColorMapped(const std::vector<vec4>& colors, real space) :
		mColors(colors), mSpace(space) {
		assert(colors.size() <= MAX_COLOR_POINTS);
	}

	virtual auto colors()const ->std::vector<vec4> {
		return mColors;
	}

	virtual auto space()const ->real {
		return mSpace;
	}
private:
	std::vector<vec4> mColors;
	real mSpace;
};