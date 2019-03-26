#include "Mesh.hpp"

#include "Utility.hpp"

Mesh::Mesh(const std::vector<Triangle>& triangles) : 
	mPrimitiveType(PrimitiveType::TriangleList)
{
	for (auto triangle : triangles) {
		auto points = triangle.points();

		mVertices.push_back({ points[0] });
		mVertices.push_back({ points[1] });
		mVertices.push_back({ points[2] });

		unsigned int index = (unsigned int)mIndices.size();

		mIndices.push_back(index + 0);
		mIndices.push_back(index + 1);
		mIndices.push_back(index + 2);
	}
}

auto Mesh::vertices() -> std::vector<Vertex>&
{
	return mVertices;
}

auto Mesh::indices() -> std::vector<unsigned int>&
{
	return mIndices;
}

auto Mesh::primitiveType() -> PrimitiveType
{
	return mPrimitiveType;
}

auto Mesh::triangles() -> std::vector<Triangle>
{
	if (mPrimitiveType == PrimitiveType::TriangleList) {
		assert(mIndices.size() % 3 == 0);

		std::vector<Triangle> triangles;

		for (size_t index = 0; index < mIndices.size(); index += 3)
			triangles.push_back(Triangle(
				mVertices[mIndices[index + 0]].Position,
				mVertices[mIndices[index + 1]].Position,
				mVertices[mIndices[index + 2]].Position
			));

		return triangles;
	}

	throw new std::exception("primitive type is not supported.");
}

float Mesh::volume()
{
	if (mPrimitiveType == PrimitiveType::TriangleList) {
		assert(mIndices.size() % 3 == 0);

		float volume = 0;

		for (size_t index = 0; index < mIndices.size(); index += 3) {
			glm::vec3 point0 = mVertices[mIndices[index + 0]].Position;
			glm::vec3 point1 = mVertices[mIndices[index + 1]].Position;
			glm::vec3 point2 = mVertices[mIndices[index + 2]].Position;

			volume = volume + glm::dot(point0, glm::cross(point1, point2));
		}

		return std::abs(volume) / 6.0f;
	}

	throw new std::exception("primitive type is not supported.");
}

Mesh Mesh::Cube(glm::vec3 size)
{
	std::vector<Vertex> vertices(8);
	std::vector<unsigned int> indices(36);

	float halfwidth = 0.5f * size.x;
	float halfheight = 0.5f * size.y;
	float halfdepth = 0.5f * size.z;

	vertices[0].Position = glm::vec3(-halfwidth, -halfheight, -halfdepth);
	vertices[1].Position = glm::vec3(-halfwidth, halfheight, -halfdepth);
	vertices[2].Position = glm::vec3(halfwidth, halfheight, -halfdepth);
	vertices[3].Position = glm::vec3(halfwidth, -halfheight, -halfdepth);
	vertices[4].Position = glm::vec3(-halfwidth, -halfheight, halfdepth);
	vertices[5].Position = glm::vec3(-halfwidth, halfheight, halfdepth);
	vertices[6].Position = glm::vec3(halfwidth, halfheight, halfdepth);
	vertices[7].Position = glm::vec3(halfwidth, -halfheight, halfdepth);

	for (size_t i = 0; i < 8; i++) {
		vertices[i].TexCoord.x = Utility::sign(vertices[i].Position.x);
		vertices[i].TexCoord.y = Utility::sign(vertices[i].Position.y);
		vertices[i].TexCoord.z = Utility::sign(vertices[i].Position.z);
	}

	indices = {
		0,2,1,0,3,2,4,5,6,4,6,7,
		4,1,5,4,0,1,3,6,2,3,7,6,
		1,6,5,1,2,6,4,3,0,4,7,3 };

	return Mesh(vertices, indices);
}
