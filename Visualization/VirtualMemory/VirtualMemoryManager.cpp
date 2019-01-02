#include "VirtualMemoryManager.hpp"

void VirtualMemoryManager::initialize(const std::string &fileName)
{
	//open volume file
	mFile.open(fileName, std::ios::binary);


}
