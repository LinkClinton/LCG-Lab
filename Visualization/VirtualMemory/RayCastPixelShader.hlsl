#include "ShaderInclude.hlsl"

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

    int3 blockCount = PAGE_SIZE_XYZ * MultiResolutionSize[level];
    int3 blockAddress = blockCount * position;

    //the block id is equal the x + y * row pitch + z * depth pitch + MultiResolutionBlockBase[level]
    //MultiResolutionBlockBase means the block count sum of 0 -> level - 1
    int rowPitch = blockCount.x;
    int depthPitch = rowPitch * blockCount.y;
    int blockID = MultiResolutionBlockBase[level] + blockAddress.x + blockAddress.y * rowPitch + blockAddress.z * depthPitch;

    //set data
    BlockCacheMissArrayRWTexture[int3(hashTableIndex.xy, count)] = blockID;
}

float sampleVolume(float3 position, int level, int reportCount, 
    int3 hashTableIndex)
{
    float sample = 0.0f;

    int3 resolutionSize = MultiResolutionSize[level];

    //compute the address of position in the directory texture
    //and get the next virtual address
    int3 directoryAddress = MultiResolutionBase[level] + position * resolutionSize;
    uint4 directoryEntry = DirectoryCacheTexture.Load(int4(directoryAddress, 0));
    
    //mapped, we continue
    if (directoryEntry.w == MAPPED)
    {
        //get the block count of level
        //so the address of current position is entry + (position * block count) % PAGE_SIZE_XYZ
        int blockCount = PAGE_SIZE_XYZ * resolutionSize.x * resolutionSize.y * resolutionSize.z;
        int3 pageTableAddress = directoryEntry.xyz * PAGE_SIZE_XYZ + (position * blockCount) % PAGE_SIZE_XYZ;

        uint4 pageTableEntry = PageCacheTexture.Load(int4(pageTableAddress, 0));
        
        //mapped, we continue
        if (pageTableEntry.w == MAPPED)
        {
            //get the voxel count of level
            //so the address of current position is entry + (position * voxel count) % BLOCK_SIZE_XYZ
            int voxelCount = BLOCK_SIZE_XYZ * blockCount;
            int3 blockTableAddress = pageTableEntry.xyz * BLOCK_SIZE_XYZ + (position * voxelCount) % BLOCK_SIZE_XYZ;

            //record the block we access, for LRU system
            BlockCacheUsageStateRWTexture[pageTableEntry.xyz] = 1;

            sample = BlockCacheTexture.Load(int4(blockTableAddress, 0));
        } else reportCacheMiss(position, level, reportCount, hashTableIndex);
    } else reportCacheMiss(position, level, reportCount, hashTableIndex);

    return sample;
}

#define STEP_SIZE 0.003
#define MAX_LOOP 100
#define LIGHT 5

float4 main(InputPixel input) : SV_Target
{
    //init
    int reportCount = 0;
    int3 hashTableIndex = int3(
    input.mSVPosition.x / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY, 
    input.mSVPosition.y / BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY, 0);

    float3 dir = normalize(input.mPosition - EyePosition[0].xyz);
    
    float4 color = float4(0, 0, 0, 0);
    float3 position = input.mTexcoord;

    [loop]
    for (int i = 0; i < MAX_LOOP; i++)
    {
        float4 sample = sampleVolume(position, 0, reportCount, hashTableIndex) * STEP_SIZE * LIGHT;

        color = (1 - sample) * color + sample;

        position = position + dir * STEP_SIZE;

        if (outLimit(position) == true || color.a >= 1.0f)
            break;
    }

    return color;
}