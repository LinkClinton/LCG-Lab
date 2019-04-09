#include "ShaderInclude.hlsl"

InputPixel main(InputVertex input)
{
	InputPixel pixel;

	//transform vertex
	pixel.mPosition = mul(float4(input.mPosition, 1.0f), WorldTranform).xyz;
	pixel.mSVPosition = mul(float4(pixel.mPosition, 1.0f), CameraTransform);
	pixel.mSVPosition = mul(pixel.mSVPosition, ProjectTransform);
	pixel.mTexcoord = input.mTexcoord;

	return pixel;
}