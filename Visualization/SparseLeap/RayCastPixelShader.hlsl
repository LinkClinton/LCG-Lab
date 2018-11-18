#include "ShaderHeader.hlsl"

#define STEP_SIZE 0.05f

#define MAX_SAMPLE_COUNT_PER_EVENT 500

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
            float3 texCoord = input.TexCoord + dir * (depth - originDepth) / cube_size;

            if (result.a >= 1.0f || outLimit(texCoord) == true)
                return result;

            float4 sampleColor = float4(1, 1, 1, VolumeTexture.Sample(Sampler, texCoord).x * STEP_SIZE);
            
            if (sampleColor.a >= 37.0f / 255.0f * 0.01f)
                result = float4(1, 1, 1, 1) * sampleColor.a + (1 - sampleColor.a) * result;
            
            depth += STEP_SIZE;

            if (depth >= raySegmentDepth)
                break;
        }

        depth = raySegmentDepth;
        boxType = raySegmentBoxType;
    }

    return result;
}