#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include <string>

#include <glm/glm.hpp>

#include "PageTable.hpp"
#include "BlockTable.hpp"
#include "PageDirectory.hpp"

#define BLOCK_CACHE_XYZ 10
#define BLOCK_CACHE_SIZE BLOCK_CACHE_XYZ * BLOCK_CACHE_XYZ * BLOCK_CACHE_XYZ

#undef min
#undef max

class CPUMemoryTestUnit {
private:
	PageDirectory* mPageDirectory;
	PageTable* mPageTable;
	BlockTable* mBlockTable;

	std::string mVolumeName;
	std::ifstream mVolumeFile;
	Size mSize;
public:
	CPUMemoryTestUnit(const std::string &volumeName, const Size &size) {
		std::ios::sync_with_stdio(false);
		
		std::vector<Size> resolutionSize;

		resolutionSize.push_back(Size(10, 10, 10));

		mBlockTable = new BlockTable(5);
		mPageTable = new PageTable(1, mBlockTable);
		mPageDirectory = new PageDirectory(resolutionSize, mPageTable);

		mVolumeName = volumeName;
		mSize = size;

		mVolumeFile.open(volumeName, std::ios::binary);
	}

	~CPUMemoryTestUnit() {
		delete mBlockTable;
		delete mPageTable;
		delete mPageDirectory;

		mVolumeFile.close();
	}

	void initialize() {
		PageCache::setPageCacheSize(Size(10, 10, 10));
		BlockCache::setBlockCacheSize(Size(10, 10, 10));
	}

	void run(int testCase) {
		int rightCase = 0;

		//random engine
		std::default_random_engine random;
		
		std::uniform_real_distribution<double> randomRange(0, 1);

		//start test
		for (int i = 0; i < testCase; i++) {
			//random position
			glm::vec3 position = glm::vec3(
				randomRange(random),
				randomRange(random),
				randomRange(random));

			//sample
			auto standByte = getAddressFromFile(position);
			auto testByte = getAddress(position);

			assert(standByte == testByte);

			if (standByte == testByte) rightCase++;
		}

		std::cout << "TestCase = " << testCase << std::endl;
		std::cout << "RightCase = " << rightCase << std::endl;
	}

	auto mapAddress(const glm::vec3 &position) {
		//get block cache size 
		auto blockSize = BlockCache::getBlockCacheSize();

		//get block address in the file
		auto fileAddress = Helper::multiple(mSize, position);
		
		//compute the entry position of block cache
		auto blockEntry = VirtualAddress(
			fileAddress.X / blockSize.X,
			fileAddress.Y / blockSize.Y,
			fileAddress.Z / blockSize.Z);
		blockEntry = Helper::multiple(blockEntry, blockSize);

		//static buffer for reading block cache
		static byte buffer[BLOCK_CACHE_SIZE];
	
		int rowPitch = mSize.X;
		int depthPitch = mSize.X * mSize.Y;
		int bufferPosition = 0;

		//compute the end position range
		//we need to avoid to read the block out of range
		Size endPositionRange = Size(
			std::min(blockEntry.X + blockSize.X, mSize.X),
			std::min(blockEntry.Y + blockSize.Y, mSize.Y),
			std::min(blockEntry.Z + blockSize.Z, mSize.Z)
		);

		int length = endPositionRange.X - blockEntry.X;

		//read buffer
		//for volume: it made from surface(index z)
		//for surface: it made from line(index y)
		//so we need to enumerate the z first, it means enumerate the surface of block cache
		//then we need to enumerate the y second, it menas enumerate the line of surface
		//at last, we read the data to buffer
		for (int z = blockEntry.Z; z < endPositionRange.Z; z++) {
			for (int y = blockEntry.Y; y < endPositionRange.Y; y++) {
				//compute the file position offset
				//compute the read block size
				int offset = z * depthPitch + y * rowPitch + blockEntry.X;
			
				assert(bufferPosition + blockSize.X <= (BLOCK_CACHE_SIZE));
			
				//seek position and read
				mVolumeFile.seekg(offset);
				mVolumeFile.read((char*)&buffer[bufferPosition], length);
				
				//next
				bufferPosition += blockSize.X;
			}

			//skip the empty data
			if (endPositionRange.Y < blockEntry.Y + blockSize.Y)
				bufferPosition += blockSize.X * (blockEntry.Y + blockSize.Y - endPositionRange.Y);
		}

		BlockCache* result = nullptr;

		//map data
		mPageDirectory->mapAddress(0, position, result = new BlockCache(BlockCache::getBlockCacheSize(), blockEntry, buffer));

		return result;
	}

	auto getAddress(const glm::vec3 &position) -> byte {
		//query block cache
		auto blockCache = mPageDirectory->queryAddress(0, position);
		auto blockSize = BlockCache::getBlockCacheSize();

		if (blockCache == nullptr) blockCache = mapAddress(position);

		//get block address in the file
		auto fileAddress = Helper::multiple(mSize, position);

		//if position.xyz is one, the address will be out of range
		//so we need to limit the address
		if (fileAddress.X == mSize.X) fileAddress.X = mSize.X - 1;
		if (fileAddress.Y == mSize.Y) fileAddress.Y = mSize.Y - 1;
		if (fileAddress.Z == mSize.Z) fileAddress.Z = mSize.Z - 1;

		//get block address
		auto blockAddress = VirtualAddress(
			fileAddress.X % blockSize.X,
			fileAddress.Y % blockSize.Y,
			fileAddress.Z % blockSize.Z);

		//get byte data
		return blockCache->getAddress(blockAddress);
	}

	auto getAddressFromFile(const glm::vec3 &position) -> byte {
		//get address
		auto address = Helper::multiple(mSize, position);

		//if position.xyz is one, the address will be out of range
		//so we need to limit the address
		if (address.X == mSize.X) address.X = mSize.X - 1;
		if (address.Y == mSize.Y) address.Y = mSize.Y - 1;
		if (address.Z == mSize.Z) address.Z = mSize.Z - 1;

		char buffer = 0;
		int mRowPitch = mSize.X;
		int mDepthPitch = mSize.X * mSize.Y;

		//seek position and read
		mVolumeFile.seekg(address.Z * mDepthPitch + address.Y * mRowPitch + address.X);
		mVolumeFile.read(&buffer, 1);

		return buffer;
	}

	static void writeTestVolumeFile(std::string fileName, Size range) {
		//disable sync
		std::ios::sync_with_stdio(false);
		
		//create file and open
		std::ofstream volumeFile;
		volumeFile.open(fileName, std::ios::binary);

		//random number
		std::default_random_engine random;
		std::uniform_real_distribution<double> randomRange(0, 1);

		long long size = (long long)range.X * (long long)range.Y * (long long)range.Z;
		
		//write file
		for (long long i = 0; i < size; i++) {
			byte output = (byte)(randomRange(random) * 255);

			volumeFile.put(output);
		}
		volumeFile.close();
	}
};