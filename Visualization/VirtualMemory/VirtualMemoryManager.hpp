#pragma once

#include "PageTable.hpp"
#include "BlockTable.hpp"
#include "PageDirectory.hpp"
#include "GPUPageTable.hpp"
#include "GPUBlockTable.hpp"
#include "GPUPageDirectory.hpp"
#include "SharedTexture3D.hpp"
#include "SparseLeapManager.hpp"

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
	PageDirectory* mDirectoryCache = nullptr;
	PageTable* mPageCacheTable = nullptr;
	BlockTable* mBlockCacheTable = nullptr;

	//GPU resource
	GPUPageDirectory* mGPUDirectoryCache = nullptr;
	GPUPageTable* mGPUPageCacheTable = nullptr;
	GPUBlockTable* mGPUBlockCacheTable = nullptr;

	//for cache
	SharedTexture3D* mBlockCacheUsageStateTexture = nullptr;
	SharedTexture3D* mBlockCacheMissArrayTexture = nullptr;

	ConstantBuffer* mMultiResolutionSizeBuffer = nullptr;
	ConstantBuffer* mMultiResolutionBaseBuffer = nullptr;
	ConstantBuffer* mMultiResolutionBlockBaseBuffer = nullptr;

	UnorderedAccessUsage* mBlockCacheUsageStateUsage = nullptr;
	UnorderedAccessUsage* mBlockCacheMissArrayUsage = nullptr;

	SparseLeapManager* mSparseLeapManager;

	void analyseFile(const std::string& fileName);

	void mapAddressToGPU(int resolution, const glm::vec3& position, BlockCache* block) const;
public:
	VirtualMemoryManager(Factory* factory, Graphics* graphics, SparseLeapManager* sparseLeapManager, int width, int height) :
		mFactory(factory), mGraphics(graphics), mResolutionWidth(width), mResolutionHeight(height), mSparseLeapManager(sparseLeapManager)
	{}

	void initialize(const std::string &fileName, const std::vector<glm::vec3> &resolutions);

	void solveCacheMiss();

	void finalize();

	void mapAddress(int resolution, int blockID);

	void loadBlock(int resolution, const VirtualAddress &blockAddress, BlockCache & output);

	auto detectResolutionLevel(float ratio) -> int;

	auto getPageDirectory() const -> GPUPageDirectory*;

	auto getPageTable() const -> GPUPageTable*;

	auto getBlockTable() const -> GPUBlockTable*;

	auto getMultiResolutionSizeBuffer() const -> ConstantBuffer*;

	auto getMultiResolutionBaseBuffer() const -> ConstantBuffer*;

	auto getMultiResolutionBlockBaseBuffer() const -> ConstantBuffer*;

	auto getUnorderedAccessUsage() const -> std::vector<UnorderedAccessUsage*>;
};