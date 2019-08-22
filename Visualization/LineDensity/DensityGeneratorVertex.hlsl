#include "DensityGeneratorInclude.hlsli"

float4 main(float3 position : POSITION, uint id : SV_INSTANCEID) : SV_POSITION
{
	float4 result = float4(position.xyz, 1.0f);

	result = mul(result, transpose(InstanceBuffer[id]));
	result = mul(result, project);

	return result;
}