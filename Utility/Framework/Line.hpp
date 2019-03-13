#pragma once

#include <glm\glm.hpp>

class Line {
private:
	glm::vec3 mStart;
	glm::vec3 mEnd;
public:
	Line(const glm::vec3 &start, const glm::vec3 &end) :
		mStart(start), mEnd(end) {}

	void setStart(const glm::vec3 &start) {
		mStart = start;
	}

	void setEnd(const glm::vec3 &end) {
		mEnd = end;
	}

	auto start() const -> glm::vec3 {
		return mStart;
	}

	auto end() const -> glm::vec3 {
		return mEnd;
	}

	auto length() const -> float {
		return (mEnd - mStart).length();
	}

	auto vector() const -> glm::vec3 {
		return mEnd - mStart;
	}
};