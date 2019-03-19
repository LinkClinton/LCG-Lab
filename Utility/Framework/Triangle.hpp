#pragma once

#include <glm\glm.hpp>

#include <vector>

class Triangle {
private:
	glm::vec3 mPoints[3];
public:
	Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
		: mPoints{ v0, v1, v2 } {}

	Triangle(const std::vector<glm::vec3> &points) {
		assert(points.size() == 3);

		memcpy(mPoints, points.data(), sizeof(mPoints));
	}

	auto points() const -> std::vector<glm::vec3> {
		auto result = std::vector<glm::vec3>(3);

		memcpy(result.data(), mPoints, sizeof(mPoints));

		return result;
	}
};