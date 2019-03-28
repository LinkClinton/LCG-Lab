#pragma once

#include <glm/glm.hpp>

class Plane {
private:
	glm::vec3 mNormal;
	float mDistance;
public:
	Plane(const glm::vec3 &normal = glm::vec3(), float distance = 0) :
		mNormal(normal), mDistance(distance) {}

	void setNormal(const glm::vec3 &normal) {
		mNormal = normal;
	}

	void setDistance(float distance) {
		mDistance = distance;
	}

	auto distance(const glm::vec3 &point) const {
		return glm::dot(point, mNormal) - mDistance;
	}

	auto normal() const -> glm::vec3 {
		return mNormal;
	}

	auto distance() const -> float const{
		return mDistance;
	}
};
