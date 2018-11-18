#pragma pack_matrix(row_major) 

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
    matrix EyePos;
};

SamplerState Sampler : register(s0);

Texture3D VolumeTexture : register(t2);


