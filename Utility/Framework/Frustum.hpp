#pragma once

#include <vector>

#include "Plane.hpp"
#include "Triangle.hpp"
#include "IntersectionSolver.hpp"

#undef near
#undef far

enum class FrustumPlane {
	Left, Top, Right, Bottom, Near, Far
};

class Frustum {
private:
	union {
		struct {
			Plane mLeft;
			Plane mTop;
			Plane mRight;
			Plane mBottom;
			Plane mNear;
			Plane mFar;
		};

		Plane mPlanes[6];
	};

	static glm::vec3 IntersectPlanes(const Plane &plane0, const Plane &plane1, const Plane &plane2);

	static auto TriangulatePoints(const std::vector<glm::vec3> &points) -> std::vector<Triangle>;
public:
	Frustum(
		const Plane &left, const Plane &top,
		const Plane &right, const Plane &bottom,
		const Plane &near, const Plane &far) :
		mLeft(left), mTop(top), mRight(right), mBottom(bottom), mNear(near), mFar(far) { }

	Frustum(const std::vector<Plane> &planes) {
		assert(planes.size() == 6);

		memcpy(mPlanes, planes.data(), sizeof(mPlanes));
	}

	auto clip(const Triangle &triangle) const -> std::vector<Triangle>;
	auto clip(const std::vector<Triangle> &triangles) const -> std::vector<Triangle>;

	Plane left() const { return mLeft; }
	Plane top() const { return mTop; }
	Plane right() const { return mRight; }
	Plane bottom() const { return mBottom; }
	Plane near() const { return mNear; }
	Plane far() const { return mFar; }

	auto corners() const -> std::vector<glm::vec3>;
};