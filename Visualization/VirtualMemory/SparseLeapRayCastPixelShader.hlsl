#include "ShaderInclude.hlsl"

#define eps 0.001f

bool outLimit(float3 texCoord)
{
	if (texCoord.x < 0.0f || texCoord.x > 1.0f)
		return true;
	if (texCoord.y < 0.0f || texCoord.y > 1.0f)
		return true;
	if (texCoord.z < 0.0f || texCoord.z > 1.0f)
		return true;

	return false;
}

float limitFloat(float value, float maxLimit)
{
	if (value >= maxLimit) value = maxLimit - eps;

	return value;
}

uint3 limitUint3(uint3 value, uint3 maxLimit)
{
	if (value.x >= maxLimit.x) value.x = maxLimit.x - 1;
	if (value.y >= maxLimit.y) value.y = maxLimit.y - 1;
	if (value.z >= maxLimit.z) value.z = maxLimit.z - 1;

	return value;
}

void reportCacheMiss(float3 position, int level, int reportCount,
	int3 hashTableIndex)
{
	//limit the number of cache miss per frame
	if (reportCount == BLOCK_RAY_CACHE_MISS_MAX_PER_FRAME) return;

	//report cache miss
	//allocate count
	int count = (++BlockCacheMissArrayRWTexture[hashTableIndex]).x;

	if (count >= BLOCK_HASH_TABLE_ARRAY_SIZE)
	{
		BlockCacheMissArrayRWTexture[hashTableIndex] = BLOCK_HASH_TABLE_ARRAY_SIZE - 1;
		return;
	}

	int3 blockCount = PAGE_SIZE_XYZ * MultiResolutionSize[level].xyz;
	int3 blockAddress = blockCount * position;

	if (blockAddress.x == blockCount.x) blockAddress.x = blockAddress.x - 1;
	if (blockAddress.y == blockCount.y) blockAddress.y = blockAddress.y - 1;
	if (blockAddress.z == blockCount.z) blockAddress.z = blockAddress.z - 1;

	//the block id is equal the x + y * row pitch + z * depth pitch + MultiResolutionBlockBase[level]
	//MultiResolutionBlockBase means the block count sum of 0 -> level - 1
	int rowPitch = blockCount.x;
	int depthPitch = rowPitch * blockCount.y;
	int blockID = MultiResolutionBlockBase[level].x + blockAddress.x + blockAddress.y * rowPitch + blockAddress.z * depthPitch;

	//set data
	BlockCacheMissArrayRWTexture[int3(hashTableIndex.xy, count)] = blockID;
}

float sampleVolume(float3 position, int level, int reportCount,
	int3 hashTableIndex)
{
	float sample = 0.0f;

	uint3 resolutionSize = MultiResolutionSize[level].xyz;

	//compute the address of position in the directory texture
	//and get the next virtual address
	uint3 directoryAddress = MultiResolutionBase[level].xyz + limitUint3(position * resolutionSize, resolutionSize);
	uint4 directoryEntry = DirectoryCacheTexture.Load(int4(directoryAddress, 0));
	

	//mapped, we continue
	if (directoryEntry.w == MAPPED)
	{
		//get the block count of level
		//so the address of current position is entry + (position * block count) % PAGE_SIZE_XYZ
		uint3 blockCount = PAGE_SIZE_XYZ * resolutionSize;
		uint3 pageTableAddress = directoryEntry.xyz * PAGE_SIZE_XYZ + limitUint3(position * blockCount, blockCount) % PAGE_SIZE_XYZ;

		uint4 pageTableEntry = PageCacheTexture.Load(int4(pageTableAddress, 0));

		//mapped, we continue
		if (pageTableEntry.w == MAPPED)
		{
			//get the voxel count of level
			//so the address of current position is entry + (position * voxel count) % BLOCK_SIZE_XYZ
			uint3 voxelCount = BLOCK_SIZE_XYZ * blockCount;
			uint3 blockTableAddress = pageTableEntry.xyz * BLOCK_SIZE_XYZ + limitUint3(position * voxelCount, voxelCount) % BLOCK_SIZE_XYZ;

			//record the block we access, for LRU system
			BlockCacheUsageStateRWTexture[pageTableEntry.xyz] = 1;

			sample = BlockCacheTexture.Load(int4(blockTableAddress, 0)).x;
		}
		else reportCacheMiss(position, level, reportCount, hashTableIndex);
	}
	else reportCacheMiss(position, level, reportCount, hashTableIndex);

	return sample;
}

#define STEP_SIZE 0.01
#define MAX_SAMPLE_COUNT_PER_EVENT 1000
#define LIGHT 5

float4 main(InputPixel input) : SV_Target
{
	//init
	int reportCount = 0;
	int3 hashTableIndex = int3(
	input.mSVPosition.x / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY,
	input.mSVPosition.y / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY, 0);

	int resolutionLevel = (int)RenderConfig[1].x;

	float3 dir = normalize(input.mPosition - RenderConfig[0].xyz);

	float4 color = float4(0, 0, 0, 0);

	float depth = distance(input.mPosition.xyz, RenderConfig[0].xyz); //depth
	float originDepth = distance(input.mPosition.xyz, RenderConfig[0].xyz); //depth

	uint raySegmentListCount = RaySegmentListCountTexture[uint2(input.mSVPosition.xy)];

	uint boxType = BOX_TYPE_EMPTY; //origin type

	float alphaLimit = EMPTY_LIMIT * STEP_SIZE * LIGHT; //we do not sample the value less than EMPTY_LIMIT
	float alphaScale = STEP_SIZE * LIGHT;  //scale for rendering
	float step = STEP_SIZE * length(float3(100, 100, 100));
	float3 texCoordStep = dir / float3(100, 100, 100); //texcoord step

	[loop]
	for (uint i = 0; i < raySegmentListCount; i++)
	{
		uint3 location = uint3(input.mSVPosition.xy, i); //ray segment list location

		float raySegmentDepth = RaySegmentListDepthTexture[location];
		uint raySegmentBoxType = RaySegmentListBoxTypeTexture[location];

		//depth is not right
		if (depth >= raySegmentDepth)
		{
			boxType = raySegmentBoxType;
			continue;
		}

		//skip
		if (boxType == BOX_TYPE_EMPTY)
		{
			depth = raySegmentDepth;
			boxType = raySegmentBoxType;
			continue;
		}

		[loop]
		for (int j = 0; j < MAX_SAMPLE_COUNT_PER_EVENT; j++)
		{
			//find the texcoord
			float3 texCoord = input.mTexcoord + texCoordStep * (depth - originDepth);

			if (color.a >= 1.0f) return color;

			//sample
			float sampleColor = sampleVolume(texCoord, resolutionLevel, reportCount, hashTableIndex).x * alphaScale;

			//cull the sample that value less than EMPTY_LIMIT
			if (sampleColor >= alphaLimit)
				color = float4(1, 1, 1, 1) * sampleColor + (1 - sampleColor) * color;

			//step
			depth += step;

			if (depth >= raySegmentDepth) break;
		}

		depth = raySegmentDepth;
		boxType = raySegmentBoxType;
	}

	return color;
}