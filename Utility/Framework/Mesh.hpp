#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PrimitiveType.hpp"
#include "Triangle.hpp"

class Mesh {
public:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 TexCoord;
	};
private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;

	PrimitiveType mPrimitiveType;
public:
	Mesh() : mPrimitiveType(PrimitiveType::TriangleList) {}

	Mesh(const std::vector<Vertex> &vertices, 
		const std::vector<unsigned int> &indices) :
		mVertices(vertices), mIndices(indices), 
		mPrimitiveType(PrimitiveType::TriangleList) {}

	Mesh(const std::vector<Triangle> &triangles);

	auto vertices() -> std::vector<Vertex>&;

	auto indices() -> std::vector<unsigned int>&;

	auto primitiveType() -> PrimitiveType;

	auto triangles() -> std::vector<Triangle>;

	float volume();

	static Mesh Cube(glm::vec3 size);
};