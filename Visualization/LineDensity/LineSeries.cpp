#include "LineSeries.hpp"

#include <random>

#undef min

LineSeries::LineSeries(const std::vector<vec2>& line_points, vec4 color) :
	mLinePoints(line_points), mColor(color) {

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

auto LineSeries::line_transform(size_t index, real width) const -> mat4 {
	auto matrix = mat4(1);

	const auto end = mLinePoints[index + 1];
	const auto start = mLinePoints[index];
	const auto vector = end - start;

	matrix = glm::translate(matrix, vec3(start.x, start.y, 0));
	matrix = glm::rotate(matrix, glm::atan(vector.y, vector.x), vec3(0, 0, 1));
	matrix = glm::translate(matrix, vec3(0, -width * 0.5f, 0));
	matrix = glm::scale(matrix, vec3(glm::length(vector), width, 1));

	return matrix;
}

auto LineSeries::line_transform_with_scale(size_t index, vec2 scale, real width) const -> mat4 
{
	auto matrix = mat4(1);

	const auto end = mLinePoints[index + 1] * scale;
	const auto start = mLinePoints[index] * scale;
	const auto vector = end - start;

	matrix = glm::translate(matrix, vec3(start.x, start.y, 0));
	matrix = glm::rotate(matrix, glm::atan(vector.y, vector.x), vec3(0, 0, 1));
	matrix = glm::translate(matrix, vec3(0, -width * 0.5f, 0));
	matrix = glm::scale(matrix, vec3(glm::length(vector), width, 1));

	return matrix;
}

auto LineSeries::size() const -> size_t 
{
	return mLinePoints.size() - 1;
}

auto LineSeries::color() const -> vec4 
{
	return mColor;
}

auto LineSeries::random_make(size_t size, real width_limit, real height_limit) -> LineSeries {
	static std::random_device device;
	static std::mt19937 engine(0);

	std::vector<vec2> lines;

	const auto space = width_limit / static_cast<real>(size);

	auto height = 0.0f;

	for (size_t i = 0; i <= size; i++) {
		lines.push_back({
			i * space,
			height = Utility::clamp(
				height + std::uniform_real_distribution<real>(-0.5f, 0.5f)(engine) * height_limit * 0.5f, 
			1.0f, height_limit - 1.0f)
			});
	}

	const std::uniform_real_distribution<real> gen_color(0.0f, 1.0f);

	return { lines, vec4(gen_color(engine), gen_color(engine), gen_color(engine), 1.0f) };
}
