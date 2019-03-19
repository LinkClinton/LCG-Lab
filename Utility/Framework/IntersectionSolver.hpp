#pragma once

#include "Line.hpp"
#include "Plane.hpp"

struct IntersectionResult {
	bool IsIntersection;

	IntersectionResult(bool isIntersection = false) : IsIntersection(isIntersection) {}
};

struct LineIntersectionResult : IntersectionResult {
	float Distance; //the distance from start
	glm::vec3 Point;

	LineIntersectionResult(bool isIntersection = false, float distance = 0, const glm::vec3 &point = glm::vec3()) 
		: IntersectionResult(isIntersection), Distance(distance), Point(point) {}
};

class IntersectionSolver {
public:
	static auto IntersectLineAndPlane(const Line &line, const Plane &plane) -> LineIntersectionResult;
};