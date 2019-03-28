#pragma once

#include "PageTable.hpp"
#include "BlockTable.hpp"
#include "PageDirectory.hpp"
#include "GPUPageTable.hpp"
#include "GPUBlockTable.hpp"
#include "GPUPageDirectory.hpp"
#include "SharedMacro.hpp"
#include "SharedTexture3D.hpp"

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

	std::vector<glm::vec3> mResolution;
	std::vector<Size> mVoxelCount;
	std::vector<Size> mReadBlockSize;

	//used for buffer update
	std::vector<Size> mMultiResolutionSize;
	std::vector<VirtualAddress> mMultiResolutionBase;
	std::vector<unsigned int> mMultiResolutionBlockEnd;
	std::vector<unsigned int> mMultiResolutionBlockBase;
	std::vector<unsigned int> mMultiResolutionBlockCount;

	//CPU resource
	PageDirectory* mDirectoryCache;
	PageTable* mPageCacheTable;
	BlockTable* mBlockCacheTable;

	//GPU resource
	GPUPageDirectory* mGPUDirectoryCache;
	GPUPageTable* mGPUPageCacheTable;
	GPUBlockTable* mGPUBlockCacheTable;

	//for cache
	SharedTexture3D* mBlockCacheUsageStateTexture;
	SharedTexture3D* mBlockCacheMissArrayTexture;

	ConstantBuffer* mMultiResolutionSizeBuffer;
	ConstantBuffer* mMultiResolutionBaseBuffer;
	ConstantBuffer* mMultiResolutionBlockBaseBuffer;

	UnorderedAccessUsage* mBlockCacheUsageStateUsage;
	UnorderedAccessUsage* mBlockCacheMissArrayUsage;

	void analyseFile(const std::string &fileName);

	void mapAddressToGPU(int resolution, const glm::vec3 &position, BlockCache* block);
public:
	VirtualMemoryManager(Factory* factory, Graphics* graphics, int width, int height) :
		mFactory(factory), mGraphics(graphics), mResolutionWidth(width), mResolutionHeight(height) {}

	void initialize(const std::string &fileName, const std::vector<glm::vec3> &resolutions);

	void solveCacheMiss();

	void finalize();

	void mapAddress(int resolution, int blockID);

	void loadBlock(int resolution, const VirtualAddress &blockAddress, BlockCache & output);

	auto detectResolutionLevel(float ratio) -> int;

	auto getPageDirectory() -> GPUPageDirectory*;

	auto getPageTable() -> GPUPageTable*;

	auto getBlockTable() -> GPUBlockTable*;

	auto getMultiResolutionSizeBuffer() -> ConstantBuffer*;

	auto getMultiResolutionBaseBuffer() -> ConstantBuffer*;

	auto getMultiResolutionBlockBaseBuffer() -> ConstantBuffer*;

	auto getUnorderedAccessUsage() -> std::vector<UnorderedAccessUsage*>;
};