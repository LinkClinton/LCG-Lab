#pragma once

#include "Utility.hpp"

#include <vector>

class LineSeries {
public:
	LineSeries(const std::vector<vec2>& line_points);

	void push(const vec2& point);

	void pop();

	auto line(size_t index)->std::pair<vec2, vec2>;

	auto size() const -> size_t;
private:
	std::vector<vec2> mLinePoints;
};