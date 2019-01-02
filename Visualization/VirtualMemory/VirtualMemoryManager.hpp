#pragma once

#include "BlockTable.hpp"
#include "PageTable.hpp"
#include "SharedMacro.hpp"

#include <WindowsFramework.hpp>
#include <fstream>

class VirtualMemoryManager {
private:
	Factory* mFactory;
	Graphics* mGraphics;
	int mResolutionWidth;
	int mResolutionHeight;

	std::ifstream mFile;

	//cpu resource
	PageDirectory* mDirectoryCache[MULTIRESOLUTION_COUNT];
	PageTable* mPageCacheTable;
	BlockTable* mBlockCacheTable;

	//gpu resource
	Texture3D* mDirectoryCacheTexture;
	Texture3D* mPageCacheTexture;
	Texture3D* mBlockCacheTexture;
	Texture3D* mBlockCacheUsageStateTexture;
	Texture3D* mBlockCacheMissHashTableTexture;

	ResourceUsage* mDirectoryCacheUsage;
	ResourceUsage* mPageCacheUsage;
	ResourceUsage* mBlockCacheUsage;

	UnorderedAccessUsage* mBlockCacheUsageStateUsage;
	UnorderedAccessUsage* mBlockCacheMissHashTableTexture;
public:
	VirtualMemoryManager(Factory* factory, Graphics* graphics, int width, int height) :
		mFactory(factory), mGraphics(graphics), mResolutionWidth(width), mResolutionHeight(height) {}

	void initialize(const std::string &fileName);
};