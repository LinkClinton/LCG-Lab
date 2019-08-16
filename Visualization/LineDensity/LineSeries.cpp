#include "LineSeries.hpp"

LineSeries::LineSeries(const std::vector<vec2>& line_points) :
	mLinePoints(line_points) {

}

void LineSeries::push(const vec2& point) {
	mLinePoints.push_back(point);
}

void LineSeries::pop() {
	assert(mLinePoints.empty() == false);

	mLinePoints.pop_back();
}

auto LineSeries::line(size_t index) -> std::pair<vec2, vec2> {
	assert(mLinePoints.size() >= 2);

	return std::make_pair(mLinePoints[index], mLinePoints[index + 1]);
}

auto LineSeries::size() const -> size_t {
	return mLinePoints.size() - 1;
}
