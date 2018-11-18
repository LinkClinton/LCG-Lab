#include "ShaderHeader.hlsl"

float4 main(OutputData input) : SV_Target
{
    float v = input.SVPosition.z / input.SVPosition.w;
    return float4(v, v, v, v);
}