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

cbuffer MatrixBuffer : register(b0)
{
    matrix WorldTranform;
    matrix CameraTransform;
    matrix ProjectTransform;
}

Texture3D<int4> DirectoryCacheTexture : register(t0);
Texture3D<int4> PageCacheTexture : register(t1);
Texture3D<float> BlockCacheTexture : register(t2);

RWTexture3D<int> BlockCacheUsageStateRWTexture : register(u1);
RWTexture3D<int2> BlockCacheMissHashTableRWTexture : register(u2);