#include <glm\glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "TornadoVectorField.hpp"

void gen_tornado_with_glm(std::vector<glm::vec3> &vectorField, size_t x, size_t y, size_t z, int time) {
	float* tempField = new float[vectorField.size() * 3];

	gen_tornado((int)x, (int)y, (int)z, time, tempField);

	for (size_t i = 0; i < vectorField.size(); i++) {
		vectorField[i] = glm::normalize(glm::vec3(
			tempField[i * 3 + 0],
			tempField[i * 3 + 1],
			tempField[i * 3 + 2]));
	}

	delete[] tempField;
}

void step(const std::vector<glm::vec3> &vectorField, std::vector<float> &renderTexture,
	size_t sizeX, size_t sizeY, size_t sizeZ, float length, float heavy) {
	assert(renderTexture.size() == vectorField.size());

	size_t rowPitch = sizeX;
	size_t depthPitch = sizeX * sizeY;

	for (size_t i = 0; i < vectorField.size(); i++) {
		size_t x = (i % depthPitch) % rowPitch;
		size_t y = (i % depthPitch) / rowPitch;
		size_t z = (i / depthPitch);

		glm::vec3 target = glm::round(glm::vec3(x, y, z) + vectorField[i] * length);

		target = glm::clamp(target, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(sizeX - 1, sizeY - 1, sizeZ - 1));

		size_t index = (size_t)target.x + (size_t)target.y * rowPitch + (size_t)target.z * depthPitch;

		renderTexture[index] += heavy;
	}
}

int main() {
	std::string volumeName = "volume";
	
	size_t volumeX = 100;
	size_t volumeY = 100;
	size_t volumeZ = 100;
	size_t times = 5;
	float length = 1.5f;
	float heavy = 0.04f;

	std::vector<glm::vec3> vectorField(volumeX * volumeY * volumeZ);
	std::vector<float> renderTexture(volumeX * volumeY * volumeZ);

	gen_tornado_with_glm(vectorField, volumeX, volumeY, volumeZ, 0);
	
	for (size_t i = 0; i < times; i++) step(vectorField, renderTexture, volumeX, volumeY, volumeZ, length * (i + 1), heavy);

	std::ofstream file;

	file.open(volumeName, std::ios::binary);

	file.write((char*)renderTexture.data(), renderTexture.size() * sizeof(float));

	file.close();
}