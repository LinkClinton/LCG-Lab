#pragma pack_matrix(row_major) 

#include "SharedMacro.hpp"

struct InputVertex
{
    float3 mPosition : POSITION;
    float3 mTexcoord : TEXCOORD;
};

struct InputPixel
{
    float3 mPosition : POSITION;
    float3 mTexcoord : TEXCOORD;
    float4 mSVPosition : SV_POSITION;
};

SamplerState Sampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix WorldTranform;
    matrix CameraTransform;
    matrix ProjectTransform;
    matrix EyePosition;
}

cbuffer MultiResolutionSizeBuffer : register(b1)
{
    int3 MultiResolutionSize[MAX_MULTIRESOLUTION_COUNT];
}

cbuffer MultiResolutionBaseBuffer : register(b2)
{
    int3 MultiResolutionBase[MAX_MULTIRESOLUTION_COUNT];
}

Texture3D<uint4> DirectoryCacheTexture : register(t0);
Texture3D<uint4> PageCacheTexture : register(t1);
Texture3D<float> BlockCacheTexture : register(t2);

RWTexture3D<uint> BlockCacheUsageStateRWTexture : register(u1);
RWTexture3D<uint> BlockCacheMissArrayRWTexture : register(u2);