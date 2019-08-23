#include "SharpGeneratorInclude.hlsli"

output main(float3 position : POSITION, uint id : SV_INSTANCEID) 
{
	output result;

	result.position = float4(position.xyz, 1.0f);
	result.position = mul(result.position, (sharp_buffer[id].world));
	result.position = mul(result.position, project);

	result.id = id;

	return result;
}