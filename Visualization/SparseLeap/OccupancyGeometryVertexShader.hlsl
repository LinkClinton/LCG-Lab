#include "ShaderHeader.hlsl"

OutputData main(InputData input, uint id : SV_INSTANCEID)
{
    OutputData result;

    result.Position = mul(float4(input.Position, 1.0f), Instance[id].Transform);
    result.SVPosition = mul(result.Position, Camera);
    result.SVPosition = mul(result.SVPosition, Project);
    result.Project = result.SVPosition;
    
    result.TexCoord = input.TexCoord;

    result.ID = id;

    return result;
}