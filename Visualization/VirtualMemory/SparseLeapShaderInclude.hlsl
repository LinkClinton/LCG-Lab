#pragma pack_matrix(row_major) 

#include "SharedMacro.hpp"

#define Empty 0
#define	NoEmpty 1
#define	Unknown 2

#define EntryEvent 0
#define ExitEvent 1

struct InputData
{
	float3 Position : POSITION;
	float3 TexCoord : TEXCOORD;
};

struct OutputData
{
    float4 SVPosition : SV_POSITION;
    float4 Position : POSITION;
    float4 Project : POSITION1;
    float3 TexCoord : TEXCOORD;
};

cbuffer Transform : register(b0)
{
    matrix World;
    matrix Camera;
    matrix Project;
    matrix Setting;
};

SamplerState Sampler : register(s0);

Texture2D<uint> RaySegmentListCountTexture : register(t1);
Texture3D<float> RaySegmentListDepthTexture : register(t2);
Texture3D<uint> RaySegmentListBoxTypeTexture : register(t3);
Texture3D<uint> RaySegmentListEventTypeTexture : register(t4);

RWTexture2D<uint> RaySegmentListCountRWTexture : register(u1);
RWTexture3D<float> RaySegmentListDepthRWTexture : register(u2);
RWTexture3D<uint> RaySegmentListBoxTypeRWTexture : register(u3);
RWTexture3D<uint> RaySegmentListEventTypeRWTexture : register(u4);

