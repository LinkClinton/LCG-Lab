#include "SharpGeneratorInclude.hlsli"

float4 main(output input) : SV_TARGET
{
	return sharp_buffer[input.id].color;
}