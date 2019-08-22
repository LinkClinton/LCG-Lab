#include "ImageGeneratorInclude.hlsli"

float4 main(float3 position : POSITION) : SV_POSITION
{
	float4 result = float4(position.xyz, 1.0f);

	result = mul(result, world);
	result = mul(result, project);

	return result;
}