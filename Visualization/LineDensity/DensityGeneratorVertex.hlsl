#include "DensityGeneratorInclude.hlsli"

float4 main(float3 position : POSITION, uint id : SV_INSTANCEID) : SV_POSITION
{
	float4 result = float4(position.xyz, 1.0f);

	result = mul(result, InstanceBuffer[id].world);
	result = mul(result, project);

	return result;
}