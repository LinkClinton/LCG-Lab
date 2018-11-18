#include "ShaderHeader.hlsl"

OutputData main(InputData input)
{
    OutputData result;

    result.Position = mul(float4(input.Position, 1.0f), World);
    result.SVPosition = mul(result.Position, Camera);
    result.SVPosition = mul(result.SVPosition, Project);
    result.Project = result.SVPosition;

    result.TexCoord = input.TexCoord;

    return result;
}
