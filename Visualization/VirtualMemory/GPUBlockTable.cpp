#include "GPUBlockTable.hpp"

#include "SharedMacro.hpp"

GPUBlockTable::GPUBlockTable(Factory * factory, Graphics * graphics, const Size & size) :
	BlockTable(size), mFactory(factory), mGraphics(graphics), mFromTexture(nullptr)
{
	//texture size is equal the table size * block size
	auto textureSize = Helper::multiple(mSize, BlockCache::getBlockCacheSize());

	mBlockTableTexture = mFactory->createTexture3D(textureSize.X, textureSize.Y, textureSize.Z, PixelFormat::R8Unknown);
	mTextureUsage = mFactory->createResourceUsage(mBlockTableTexture, mBlockTableTexture->getPixelFormat());
}

GPUBlockTable::~GPUBlockTable()
{
	mFactory->destoryResourceUsage(mTextureUsage);
	mFactory->destoryTexture3D(mBlockTableTexture);
}

void GPUBlockTable::mallocAddress(VirtualLink * virtualLink)
{
	//CPU
	BlockTable::mallocAddress(virtualLink);

	//now we need to update the relationship in the texture(GPU memory)
	GPUHelper::modifyVirtualLinkToTexture(virtualLink, mFromTexture);
}

void GPUBlockTable::clearUpAddress(const VirtualAddress & address)
{
	//before CPU version, we need to clear up the relationship in the texture(GPU memory)
	//we get the virtual link from page table to block table
	//because the virtual link will be removed by calling the BlockTable::clearUpAddress
	auto virtualLink = mMapRelation[getArrayIndex(address)];

	BlockTable::clearUpAddress(address);

	//now, the virtual link is reset, we need upload it to the texture
	//and we do not need to clear the block cache data in the texture
	//because the data will be covered with new data when the block cache is used
	if (virtualLink != nullptr) GPUHelper::modifyVirtualLinkToTexture(virtualLink, mFromTexture);
}

void GPUBlockTable::mapAddress(const glm::vec3 & position, const Size & size, BlockCache * blockCache, VirtualLink * virtualLink)
{
	//in fact, we can not to set the block cache in the CPU version
	//because we will delete it not only once
	//so we only upload the block cache to the texture(GPU memory) and set virtual empty block cache
	//get block size and the block cache's range([block size * address, block size * address + block size))
	auto blockSize = BlockCache::getBlockCacheSize();
	auto startRange = Helper::multiple(blockSize, virtualLink->Address);

	//update block cache
	mBlockTableTexture->update(blockCache->getDataPointer(),
		startRange.X, startRange.Y, startRange.Z,
		startRange.X + blockSize.X, startRange.Y + blockSize.Y, startRange.Z + blockSize.Z);
	
	//CPU version
	//set virtual empty block cache, because the block cache only need to upload to texture(GPU memory)
	//so we only set itself to trigger the LRU system 
	setAddress(virtualLink->Address, getAddress(virtualLink->Address));
}

auto GPUBlockTable::queryAddress(const glm::vec3 & position, const Size & size, VirtualLink * virtualLink) -> BlockCache * 
{
	//do not override
	return BlockTable::queryAddress(position, size, virtualLink);
}

auto GPUBlockTable::getTexture() -> Texture3D *
{
	return mBlockTableTexture;
}

auto GPUBlockTable::getTextureUsage() -> ResourceUsage *
{
	return mTextureUsage;
}

void GPUHelper::modifyVirtualLinkToTexture(VirtualLink * virtualLink, Texture3D * texture)
{
	//create the modify data, because we use R8G8B8A8 format, so we need use "byte"
	//because the address in the range of [0, BLOCK_COUNT_XYZ), we can use "byte"
	byte modifyData[4];
	modifyData[0] = (byte)virtualLink->Address.X;
	modifyData[1] = (byte)virtualLink->Address.Y;
	modifyData[2] = (byte)virtualLink->Address.Z;
	modifyData[3] = (byte)virtualLink->State;

	//get from address
	auto fromAddress = virtualLink->FromAddress;

	//modify the texture
	texture->update(modifyData,
		fromAddress.X, fromAddress.Y, fromAddress.Z,
		fromAddress.X + 1, fromAddress.Y + 1, fromAddress.Z + 1);
}
