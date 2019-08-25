#pragma once

#include "Utility.hpp"

#include <vector>

class LineSeries {
public:
	LineSeries() : mLinePoints(), mColor(0.0f, 0.0f, 0.0f, 1.0f) {}

	LineSeries(const std::vector<vec2>& line_points, vec4 color = vec4(0.0f,0.0f,0.0f,1.0f));

	void push(const vec2& point);

	void pop();

	auto line(size_t index)->std::pair<vec2, vec2>;

	auto line_transform(size_t index, real width = 2.0f) const -> mat4;

	auto line_transform_with_scale(size_t index, vec2 scale, real width = 2.0f) const->mat4;

	auto size() const -> size_t;

	auto color() const -> vec4;

	friend std::istream& operator>>(std::istream& in, LineSeries& lineSeries);

	friend std::ostream& operator<<(std::ostream& out, const LineSeries& lineSeries);

	static auto random_make(size_t size, real width_limit, real height_limit) -> LineSeries;

	static auto read_from_file(const std::string& fileName) -> 
		std::tuple<std::vector<LineSeries>, size_t, size_t>;

	static void save_to_file(const std::string& fileName, 
		std::tuple<const std::vector<LineSeries>&, size_t, size_t> data);
private:
	std::vector<vec2> mLinePoints;
	vec4 mColor;
};