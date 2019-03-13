#include "IntersectionSolver.hpp"
#include "Plane.hpp"
#include "Line.hpp"

auto IntersectionSolver::IntersectLineAndPlane(const Line & line, const Plane & plane) -> LineIntersectionResult
{
	float nA = glm::dot(plane.normal(), line.start());
	float nAB = glm::dot(plane.normal(), line.vector());

	if (nAB == 0) return LineIntersectionResult(false, 0);

	float distance = (plane.distance() - nA) / nAB;

	if (distance >= 0.0f && distance <= 1.0f) return LineIntersectionResult(true, distance);
	else return LineIntersectionResult(false, 0);
}
