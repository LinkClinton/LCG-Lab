#include "DensityGeneratorInclude.hlsli"

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
	int2 index0 = int2(position.xy);
	int2 count = int2(position.x, 0);

	if (CountTexture2D[count] != 0) {
		float origin = 0.0f;

		InterlockedExchange(BufferTexture2D[index0], 0, origin);

		if (origin == 0.0f) return float4(1, 0, 0, 1);

		HeatMapTexture2D[index0] = HeatMapTexture2D[index0] +
			origin / (float)CountTexture2D[count];
	}
	return float4(1, 0, 0, 1);
}