#include "LineSeries.hpp"

#include <random>

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

auto LineSeries::random_make(size_t size, real width_limit, real height_limit) -> LineSeries {
	static std::random_device device;
	static std::mt19937 engine(0);

	std::vector<vec2> lines;

	const auto space = width_limit / static_cast<real>(size + 1);

	for (size_t i = 0; i <= size; i++) {
		lines.push_back({
			std::uniform_real_distribution<real>(i * space, i * space + space)(engine),
			std::uniform_real_distribution<real>(0.0f, height_limit)(engine)
			});
	}

	return lines;
}
