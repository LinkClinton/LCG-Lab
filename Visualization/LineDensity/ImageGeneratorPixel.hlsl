#include "ImageGeneratorInclude.hlsli"

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
	float width = others[0].x;
	float height = others[0].y;
	float count = others[0].z;
	float space = others[0].w;
	float max_density = space * (count - 1);

	int2 index = int2(
		position.x * width,
		position.y * height);

	float sample_value = HeatMapTexture2D[index];
	
	if (sample_value == 0.0f) discard;
	if (sample_value >= max_density) return colors[(int)(count) - 1];

	int mapped_index = sample_value / space;
	
	return lerp(colors[mapped_index], colors[mapped_index + 1], clamp((sample_value - mapped_index * space) / space, 0.0f, 1.0f));
}