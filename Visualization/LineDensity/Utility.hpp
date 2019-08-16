#pragma once

#include <memory>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <WindowsFramework.hpp>

using real = float;
using vec2 = glm::vec<2, real, glm::defaultp>;
using vec3 = glm::vec<3, real, glm::defaultp>;
using mat4 = glm::mat<4, 4, real, glm::defaultp>;
using byte = unsigned char;

class ShaderFile {
public:
	static auto read(const std::string& fileName)->std::vector<byte> {
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);
		
		assert(file.is_open());

		const auto fileSize = size_t(file.tellg());

		std::vector<byte> fileCode(fileSize);

		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(&fileCode[0]), fileSize);
		file.close();

		return fileCode;
	}
};