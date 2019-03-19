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

	static glm::vec3 IntersectPlanes(const Plane &plane0, const Plane &plane1, const Plane &plane2) {
		//find the point the plane intersect by using Cramer's Rule
		auto D = glm::mat3(plane0.normal(), plane1.normal(), plane2.normal());
		auto A = glm::vec3(-plane0.distance(), -plane1.distance(), -plane2.distance());

		auto Dx = D, Dy = D, Dz = D;

		Dx[0] = A; Dy[0] = A; Dz[0] = A;

		//get the determinant
		auto determinant = glm::determinant(D);

		assert(determinant != 0);

		auto determinantDx = glm::determinant(Dx);
		auto determinantDy = glm::determinant(Dy);
		auto determinantDz = glm::determinant(Dz);

		return glm::vec3(
			determinantDx / determinant,
			determinantDy / determinant,
			determinantDz / determinant);
	}

	static auto TriangulatePoints(const std::vector<glm::vec3> &points) -> std::vector<Triangle> {
		assert(points.size() >= 3);

		auto importantPoint = points[0];
		auto result = std::vector<Triangle>();

		for (size_t i = 0; i < points.size() - 2; i++) {
			auto trianglePoints = std::vector<glm::vec3>(3);

			trianglePoints[0] = importantPoint;
			trianglePoints[1] = points[i + 1];
			trianglePoints[2] = points[i + 2];

			result.push_back(Triangle(trianglePoints));
		}

		return result;
	}
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

	auto clip(const Triangle &triangle) const -> std::vector<Triangle> {
		auto points = triangle.points();

		//enum plane
		for (auto plane : mPlanes) {
			assert(points.size() >= 3);

			auto newPoints = std::vector<glm::vec3>();

			//enum all edges and clip the edge
			for (size_t index = 0; index < points.size(); index++) {
				auto point0 = points[index];
				auto point1 = points[(index + 1) % points.size()];

				auto isInside0 = plane.distance(point0) >= 0;
				auto isInside1 = plane.distance(point1) >= 0;

				//outside
				if (isInside0 == false && isInside1 == false)
					continue;

				//inside
				if (isInside0 == true && isInside1 == true) {
					newPoints.push_back(point0); 
					continue;
				}

				auto intersectionResult = IntersectionSolver::IntersectLineAndPlane(Line(point0, point1), plane);
				auto newPoint = intersectionResult.Point;
				
				assert(intersectionResult.IsIntersection == true);
				
				if (isInside0 == false && isInside1 == true) {
					newPoints.push_back(newPoint);
					continue;
				}

				if (isInside0 == true && isInside1 == false) {
					newPoints.push_back(point0);
					newPoints.push_back(newPoint);
					continue;
				}
			}

			points = newPoints;
		}

		return TriangulatePoints(points);
	}

	Plane left() const { return mLeft; }
	Plane top() const { return mTop; }
	Plane right() const { return mRight; }
	Plane bottom() const { return mBottom; }
	Plane near() const { return mNear; }
	Plane far() const { return mFar; }

	auto corners() const -> std::vector<glm::vec3> {
		auto result = std::vector<glm::vec3>(8);

		result[0] = IntersectPlanes(mNear, mTop, mLeft);
		result[1] = IntersectPlanes(mNear, mTop, mRight);
		result[2] = IntersectPlanes(mNear, mBottom, mLeft);
		result[3] = IntersectPlanes(mNear, mBottom, mRight);
		result[4] = IntersectPlanes(mFar, mTop, mLeft);
		result[5] = IntersectPlanes(mFar, mTop, mRight);
		result[6] = IntersectPlanes(mFar, mBottom, mLeft);
		result[7] = IntersectPlanes(mFar, mBottom, mRight);

		return result;
	}
};