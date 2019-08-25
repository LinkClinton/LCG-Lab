#include "DensityGeneratorInclude.hlsli"

float4 main(float2 position : POSITION) : SV_POSITION
{
	float4 result = float4(position.xy, 0.0f, 1.0f);

	result = mul(result, project);

	return result;
}