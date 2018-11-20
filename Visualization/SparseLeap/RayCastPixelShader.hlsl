#include "ShaderHeader.hlsl"

#define STEP_SIZE 0.01f
#define EMPTY_LIMIT 0.145f
#define LIGHT 2

#define MAX_SAMPLE_COUNT_PER_EVENT 3000

bool outLimit(float3 texCoord)
{
    if (texCoord.x < 0.0f || texCoord.x > 1.0f)
        return true;
    if (texCoord.y < 0.0f || texCoord.y > 1.0f)
        return true;
    if (texCoord.z < 0.0f || texCoord.z > 1.0f)
        return true;

    return false;
}

float4 main(OutputData input) : SV_Target
{
    float3 cube_size = float3(3.0f, 3.0f, 3.0f);
    float3 dir = normalize(input.Position.xyz - EyePos[0].xyz);
    float depth = distance(input.Position.xyz, EyePos[0].xyz);
    float originDepth = distance(input.Position.xyz, EyePos[0].xyz);

    float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
  
    uint raySegmentListCount = RaySegmentListCountTexture[uint2(input.SVPosition.xy)];
    
    uint boxType = Empty;

    float alphaLimit = EMPTY_LIMIT * STEP_SIZE * LIGHT;
    float alphaScale = STEP_SIZE * LIGHT;
    float3 texCoordStep = dir / cube_size;

    [loop]
    for (uint i = 0; i < raySegmentListCount; i++)
    {
        uint3 location = uint3(input.SVPosition.xy, i);

        float raySegmentDepth = RaySegmentListDepthTexture[location];
        uint raySegmentBoxType = RaySegmentListBoxTypeTexture[location];

        //depth is not right
        if (depth >= raySegmentDepth)
        {
            boxType = raySegmentBoxType;
            continue;
        }

        //skip
        if (boxType == Empty)
        {
            depth = raySegmentDepth;
            boxType = raySegmentBoxType;
            continue;
        }
        
        [loop]
        for (int j = 0; j < MAX_SAMPLE_COUNT_PER_EVENT; j++)
        {
            float3 texCoord = input.TexCoord + texCoordStep * (depth - originDepth);

            if (result.a >= 1.0f)
                return result;

            float sampleColor = VolumeTexture.Sample(Sampler, texCoord).x * alphaScale;
            
            if (sampleColor >= alphaLimit)
                result = float4(1, 1, 1, 1) * sampleColor + (1 - sampleColor) * result;
            
            depth += STEP_SIZE;

            if (depth >= raySegmentDepth)
                break;
        }

        depth = raySegmentDepth;
        boxType = raySegmentBoxType;
    }

    return result;
}