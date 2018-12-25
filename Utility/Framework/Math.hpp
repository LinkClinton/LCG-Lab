#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Math {
public:
	static auto getOrthogonalVector(const glm::vec3 &vector) {
		assert(glm::length(vector) != 0);

		int zeroCount = 0;

		if (vector.x == 0) zeroCount++;
		if (vector.y == 0) zeroCount++;
		if (vector.z == 0) zeroCount++;

		if (zeroCount == 0 || zeroCount == 1) {
			if (vector.x != 0) return glm::normalize(glm::vec3(0, -vector.z, vector.y));
			if (vector.y != 0) return glm::normalize(glm::vec3(-vector.z, 0, vector.x));
			if (vector.z != 0) return glm::normalize(glm::vec3(-vector.y, vector.x, 0));
		}

		//zero count is 2
		if (vector.x == 0) return glm::vec3(1, 0, 0);
		if (vector.y == 0) return glm::vec3(0, 1, 0);
		if (vector.z == 0) return glm::vec3(0, 0, 1);

		return glm::vec3(0, 0, 0);
	}

	static auto computeRotateFromVector(const glm::vec3 &from, const glm::vec3 &to) {
		assert(glm::length(from) != 0);
		assert(glm::length(to) != 0);

		//get rotate axis and angle(u = from, v = to)
		//axis = normalize(u cross v) and angle = acos((u dot v) / (|u| * |v|))
		auto rotateAxis = (glm::cross(from, to));

		if (glm::length(rotateAxis) == 0) rotateAxis = getOrthogonalVector(from);
		else rotateAxis = glm::normalize(rotateAxis);

		auto rotateAngle = acos(glm::dot(from, to) / (glm::length(from) * glm::length(to)));

		//return 
		return glm::rotate(glm::tquat<float, glm::qualifier::highp>(1, 0, 0, 0), rotateAngle, rotateAxis);
	}
};