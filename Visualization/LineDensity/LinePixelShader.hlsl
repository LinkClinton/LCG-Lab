#include "ShaderInclude.hlsl"

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
	BufferTexture2D[int2(position.x, position.y)] = 1.0f;
	
	InterlockedAdd(CountTexture2D[int2(position.x, 0)], 1);

	return float4(1, 0, 0, 1);
}