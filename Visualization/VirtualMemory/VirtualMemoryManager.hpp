#pragma once

#include "BlockTable.hpp"
#include "PageTable.hpp"
#include "PageDirectory.hpp"
#include "SharedMacro.hpp"

#include <WindowsFramework.hpp>
#include <fstream>
#include <vector>

class VirtualMemoryManager {
private:
	Factory* mFactory;
	Graphics* mGraphics;
	int mResolutionWidth;
	int mResolutionHeight;

	std::ifstream mFile;
	Size mFileSize;

	//cpu resource
	PageDirectory* mDirectoryCache;
	PageTable* mPageCacheTable;
	BlockTable* mBlockCacheTable;

	//gpu resource
	Texture3D* mDirectoryCacheTexture;
	Texture3D* mPageCacheTexture;
	Texture3D* mBlockCacheTexture;
	Texture3D* mBlockCacheUsageStateTexture;
	Texture3D* mBlockCacheMissArrayTexture;

	ResourceUsage* mDirectoryCacheUsage;
	ResourceUsage* mPageCacheUsage;
	ResourceUsage* mBlockCacheUsage;

	UnorderedAccessUsage* mBlockCacheUsageStateUsage;
	UnorderedAccessUsage* mBlockCacheMissArrayUsage;

	void analyseFile(const std::string &fileName);

	void mapAddressToGPU(BlockCache* block, int resolutionLevel, const glm::vec3 &position);
public:
	VirtualMemoryManager(Factory* factory, Graphics* graphics, int width, int height) :
		mFactory(factory), mGraphics(graphics), mResolutionWidth(width), mResolutionHeight(height) {}

	void initialize(const std::string &fileName, const std::vector<glm::vec3> &resolutions);

	void mapAddress(int resolutionLevel, int blockID);

	auto loadBlock(int resolutionLevel, const VirtualAddress &blockAddress) -> BlockCache*;
};