#include "ShaderHeader.hlsl"

#define eps 0.0000001

void setRaySegmentList(float depth, uint type, uint eventType, uint3 location)
{
    RaySegmentListDepthRWTexture[location] = depth;
    RaySegmentListBoxTypeRWTexture[location] = type;
    RaySegmentListEventTypeRWTexture[location] = eventType;
}

void addRaySegmentList(float depth, uint type, uint eventType, uint raySegmentListCount, uint2 location)
{
    uint3 firstPosition = uint3(location, raySegmentListCount - 1);
    uint3 secondPosition = uint3(location, raySegmentListCount);
    
    //set the event
    setRaySegmentList(depth, type, eventType, secondPosition);
    RaySegmentListCountRWTexture[location] = raySegmentListCount + 1;

    //do not need merge or delete
    if (raySegmentListCount == 0) return;

    float beforeDepth = RaySegmentListDepthRWTexture[firstPosition];

    //the depth is not same
    if (abs(beforeDepth - depth) > eps) return;
    
    uint beforeEventType = RaySegmentListEventTypeRWTexture[firstPosition];
    uint beforeType = RaySegmentListBoxTypeRWTexture[firstPosition];

    //case 1, the same event type, delete the first
    if (beforeEventType == eventType)
    {
        setRaySegmentList(depth, type, eventType, firstPosition);
        RaySegmentListCountRWTexture[location] = raySegmentListCount;

        return;
    }

    //case 2, first is exit and second is entry 
    if (beforeEventType == ExitEvent && eventType == EntryEvent) // or if (eventType == EntryEvent)
    {
        //before the first
        if (raySegmentListCount == 1)
            return;

        //same, delete all
        if (type == RaySegmentListBoxTypeRWTexture[uint3(location, raySegmentListCount - 2)])
            RaySegmentListCountRWTexture[location] = raySegmentListCount - 1;

        return;
    }

    //case 3, first is entry and second is exit
    if (beforeEventType == EntryEvent && eventType == ExitEvent) //of if (eventType == ExitEvent)
        RaySegmentListCountRWTexture[location] = raySegmentListCount - 1;
}

float4 main(OutputData input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    uint2 location = uint2(input.SVPosition.xy);

    uint raySegmentListCount = RaySegmentListCountRWTexture[location];

    float depth = distance(input.Position.xyz, EyePos[0].xyz);

    //error
    if (raySegmentListCount + 1 >= MAX_RAYSEGMENT_COUNT)
        return float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    if (Instance[input.ID].Seting.x == 1 && isFrontFace == true)
        addRaySegmentList(depth, Instance[input.ID].Seting.y, EntryEvent, raySegmentListCount, location);

    if (Instance[input.ID].Seting.x == 0 && isFrontFace == false)
        addRaySegmentList(depth, Instance[input.ID].Seting.z, ExitEvent, raySegmentListCount, location);

    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}