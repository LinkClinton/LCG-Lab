#include "ShaderHeader.hlsl"

//sample step
#define STEP_SIZE 0.01f
//empty limit
#define EMPTY_LIMIT 0.145f
//light value
#define LIGHT 2

#define MAX_SAMPLE_COUNT_PER_EVENT 3000

float4 main(OutputData input) : SV_Target
{
    float3 cube_size = float3(3.0f, 3.0f, 3.0f); //cube size
    float3 dir = normalize(input.Position.xyz - EyePos[0].xyz); //direction
    float depth = distance(input.Position.xyz, EyePos[0].xyz); //depth
    float originDepth = distance(input.Position.xyz, EyePos[0].xyz); //depth

    float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f); //result
  
    uint raySegmentListCount = RaySegmentListCountTexture[uint2(input.SVPosition.xy)];
    
    uint boxType = Empty; //origin type

    float alphaLimit = EMPTY_LIMIT * STEP_SIZE * LIGHT; //we do not sample the value less than EMPTY_LIMIT
    float alphaScale = STEP_SIZE * LIGHT;  //scale for rendering
    float3 texCoordStep = dir / cube_size; //texcoord step

    [loop]
    for (uint i = 0; i < raySegmentListCount; i++)
    {
        uint3 location = uint3(input.SVPosition.xy, i); //ray segment list location

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
            //find the texcoord
            float3 texCoord = input.TexCoord + texCoordStep * (depth - originDepth); 

            if (result.a >= 1.0f)
                return result;

            //sample
            float sampleColor = VolumeTexture.Sample(Sampler, texCoord).x * alphaScale; 
            
            //cull the sample that value less than EMPTY_LIMIT
            if (sampleColor >= alphaLimit)
                result = float4(1, 1, 1, 1) * sampleColor + (1 - sampleColor) * result;
            
            //step
            depth += STEP_SIZE;

            if (depth >= raySegmentDepth)
                break;
        }

        depth = raySegmentDepth;
        boxType = raySegmentBoxType;
    }

    return result;
}