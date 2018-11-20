#include "ShaderHeader.hlsl"

#define STEP_SIZE 0.003
#define MAX_LOOP_SIZE 3000

bool OutLimit(float3 texCoord)
{
    if (texCoord.x < 0.0f || texCoord.x > 1.0f) return true;
    if (texCoord.y < 0.0f || texCoord.y > 1.0f) return true;
    if (texCoord.z < 0.0f || texCoord.z > 1.0f) return true;

    return false;
}

float4 main(OutputData input) : SV_Target
{
    float4 result = float4(0, 0, 0, 0);

    float3 dir = normalize(input.Position.xyz - float3(EyePos[0][0], EyePos[0][1], EyePos[0][2]));

    float3 position = input.TexCoord;

    [loop]
    for (int i = 0; i < MAX_LOOP_SIZE; i++)
    {
        float4 sampleColor = float4(1, 1, 1, VolumeTexture.Sample(Sampler, position).x * STEP_SIZE * 5);

        if (sampleColor.a / STEP_SIZE / 5.0f >= 37.0f / 255.0f)

        result = sampleColor * sampleColor.a + (1 - sampleColor.a) * result;

        position += dir * STEP_SIZE;

        if (OutLimit(position) == true || result.a >= 1.0f)
            break;
    }

    return result;
}