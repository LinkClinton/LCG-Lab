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

	friend std::istream& operator>>(std::istream& in, ColorMapped& colorMapped);

	friend std::ostream& operator<<(std::ostream& out, const ColorMapped& colorMapped);
private:
	std::vector<vec4> mColors;
	real mSpace;
};

inline std::istream& operator>>(std::istream& in, ColorMapped& colorMapped) {
	//1st line : size of color mapped and the space
	size_t size = 0;

	in >> size >> colorMapped.mSpace;
	//2nd - (size + 1)th line : color
	colorMapped.mColors.resize(size);
	for (auto& color : colorMapped.mColors)
		in >> color.r >> color.g >> color.b >> color.a;

	return in;
}

inline std::ostream& operator<<(std::ostream& out, const ColorMapped& colorMapped) {
	//1st line : size of color mapped and the space
	out << colorMapped.mColors.size() << " " << colorMapped.mSpace << std::endl;
	//2nd - (size + 1)th line : color
	for (const auto& color : colorMapped.mColors)
		out << color.r << " " << color.g << " " << color.b << " " << color.a << std::endl;

	return out;
}
