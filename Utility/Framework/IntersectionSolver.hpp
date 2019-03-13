#pragma once

class Line;
class Plane;

struct IntersectionResult {
	bool IsIntersection;

	IntersectionResult(bool isIntersection = false) : IsIntersection(isIntersection) {}
};

struct LineIntersectionResult : IntersectionResult {
	float Distance; //the distance from start

	LineIntersectionResult(bool isIntersection = false, float distance = 0) : IntersectionResult(isIntersection),
		Distance(distance) {}
};

class IntersectionSolver {
public:
	static auto IntersectLineAndPlane(const Line &line, const Plane &plane) -> LineIntersectionResult;
};