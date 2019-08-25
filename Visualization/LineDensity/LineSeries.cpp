#include "LineSeries.hpp"

#include <fstream>
#include <random>
#include <tuple>

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

auto LineSeries::data() -> vec2* {
	return mLinePoints.data();
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
				height + std::uniform_real_distribution<real>(-0.5f, 0.5f)(engine) * height_limit * 0.1f, 
			1.0f, height_limit - 1.0f)
			});
	}

	const std::uniform_real_distribution<real> gen_color(0.0f, 1.0f);

	return { lines, vec4(gen_color(engine), gen_color(engine), gen_color(engine), 1.0f) };
}

auto LineSeries::read_from_file(const std::string& fileName) -> std::tuple<std::vector<LineSeries>, size_t, size_t> {
	//1th line: (n) number of line series width height
	//2nd -> (1 + n) th : ni(number of lines) px0 py0 px1 py1 ... px(n + 1) py(n + 1) red green blue alpha

	std::ifstream file;
	size_t nLineSeries = 0;
	size_t width = 0;
	size_t height = 0;

	file.open(fileName);
	assert(file.is_open() == true);

	file >> nLineSeries >> width >> height;

	std::vector<LineSeries> lineSeries(nLineSeries);
	for (size_t index = 0; index < nLineSeries; index++) file >> lineSeries[index];

	file.close();

	return std::make_tuple(lineSeries, width, height);
}

void LineSeries::save_to_file(const std::string& fileName, std::tuple<const std::vector<LineSeries>&, size_t, size_t> data) {
	std::ofstream file;

	file.open(fileName);
	assert(file.is_open() == true);

	auto& lineSeries = std::get<0>(data);

	file << lineSeries.size() << " " << std::get<1>(data) << " " << std::get<2>(data) << std::endl;
	for (auto& lines : lineSeries) file << lines << std::endl;
	
	file.close();
}

std::istream& operator>>(std::istream& in, LineSeries& lineSeries) {
	size_t nLines = 0;

	//first, read the number of lines
	in >> nLines;
	//second, read the position of lines
	lineSeries.mLinePoints.resize(nLines + 1);
	for (size_t index = 0; index <= nLines; index++)
		in >> lineSeries.mLinePoints[index].x >> lineSeries.mLinePoints[index].y;
	//third, read the color of line series
	return in >> lineSeries.mColor.r >> lineSeries.mColor.g >> lineSeries.mColor.b >> lineSeries.mColor.a;
}

std::ostream& operator<<(std::ostream& out, const LineSeries& lineSeries) {
	//first, write the number of lines
	out << lineSeries.size() << " ";
	//second, write the position of lines
	for (auto& point : lineSeries.mLinePoints) out << point.x << " " << point.y << " ";
	//third, write the color of line series
	return out << lineSeries.mColor.r << " " << lineSeries.mColor.g << " " << lineSeries.mColor.b << " " << lineSeries.mColor.a;
}
