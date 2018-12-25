#pragma pack_matrix(row_major) 

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
