#pragma once

#include <WindowsFramework.hpp>

#include <fstream>

class FileManager {
public:
	static auto ReadFile(const std::string &fileName) -> std::vector<codebyte> {
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);

		auto fileSize = (size_t)file.tellg();

		std::vector<codebyte> fileCode(fileSize);

		file.seekg(0, std::ios::beg);
		file.read((char*)&fileCode[0], fileSize);
		file.close();

		return fileCode;
	}
};