#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 TexCoord;
};


class Cube {
public:
	static auto Sign(float value) -> float {
		if (value < 0) return 0;
		return 1;
	}

	static auto GetVertics(float width, float height, float depth) -> std::vector<Vertex> {
		std::vector<Vertex> result(8);

		float halfwidth = 0.5f * width;
		float halfheight = 0.5f * height;
		float halfdepth = 0.5f * depth;

		result[0].Position = glm::vec3(-halfwidth, -halfheight, -halfdepth);
		result[1].Position = glm::vec3(-halfwidth, halfheight, -halfdepth);
		result[2].Position = glm::vec3(halfwidth, halfheight, -halfdepth);
		result[3].Position = glm::vec3(halfwidth, -halfheight, -halfdepth);
		result[4].Position = glm::vec3(-halfwidth, -halfheight, halfdepth);
		result[5].Position = glm::vec3(-halfwidth, halfheight, halfdepth);
		result[6].Position = glm::vec3(halfwidth, halfheight, halfdepth);
		result[7].Position = glm::vec3(halfwidth, -halfheight, halfdepth);

		for (size_t i = 0; i < 8; i++) {
			result[i].TexCoord.x = Sign(result[i].Position.x);
			result[i].TexCoord.y = Sign(result[i].Position.y);
			result[i].TexCoord.z = Sign(result[i].Position.z);
		}

		return result;
	}

	static auto GetIndices() -> std::vector<unsigned int> {
		unsigned int indices[36] = {
			0,2,1,0,3,2,4,5,6,4,6,7,
			4,1,5,4,0,1,3,6,2,3,7,6,
			1,6,5,1,2,6,4,3,0,4,7,3
		};

		std::vector<unsigned int> result;

		for (int i = 0; i < 36; i++)
			result.push_back(indices[i]);

		return result;
	}
};