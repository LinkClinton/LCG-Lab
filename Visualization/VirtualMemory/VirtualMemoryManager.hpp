#pragma once

#include "PageTable.hpp"
#include "BlockTable.hpp"
#include "PageDirectory.hpp"
#include "GPUPageTable.hpp"
#include "GPUBlockTable.hpp"
#include "GPUPageDirectory.hpp"
#include "SharedMacro.hpp"

#include <Framework.hpp>
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

	//CPU resource
	PageDirectory* mDirectoryCache;
	PageTable* mPageCacheTable;
	BlockTable* mBlockCacheTable;

	//GPU resource
	GPUPageDirectory* mGPUDirectoryCache;
	GPUPageTable* mGPUPageCacheTable;
	GPUBlockTable* mGPUBlockCacheTable;

	//for cache
	Texture3D* mBlockCacheUsageStateTexture;
	Texture3D* mBlockCacheMissArrayTexture;

	ConstantBuffer* mMultiResolutionSizeBuffer;
	ConstantBuffer* mMultiResolutionBaseBuffer;

	UnorderedAccessUsage* mBlockCacheUsageStateUsage;
	UnorderedAccessUsage* mBlockCacheMissArrayUsage;

	void analyseFile(const std::string &fileName);

	void mapAddressToGPU(int resolution, const glm::vec3 &position, BlockCache* block);
public:
	VirtualMemoryManager(Factory* factory, Graphics* graphics, int width, int height) :
		mFactory(factory), mGraphics(graphics), mResolutionWidth(width), mResolutionHeight(height) {}

	void initialize(const std::string &fileName, const std::vector<glm::vec3> &resolutions);

	void finalize();

	void mapAddress(int resolution, int blockID);

	auto loadBlock(int resolution, const VirtualAddress &blockAddress) -> BlockCache*;

	auto getPageDirectory() -> GPUPageDirectory*;

	auto getPageTable() -> GPUPageTable*;

	auto getBlockTable() -> GPUBlockTable*;

	auto getMultiResolutionSizeBuffer() -> ConstantBuffer*;

	auto getMultiResolutionBaseBuffer() -> ConstantBuffer*;

	auto getUnorderedAccessUsage() -> std::vector<UnorderedAccessUsage*>;
};