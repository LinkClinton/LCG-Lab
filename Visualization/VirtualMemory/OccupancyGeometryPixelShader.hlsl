#include "SparseLeapShaderInclude.hlsl"

#define eps 0.0000001

void setRaySegmentList(float depth, uint type, uint eventType, uint3 location)
{
    /*int output = 0;

    InterlockedExchange(RaySegmentListDepthRWTexture[location], depth, output);
    InterlockedExchange(RaySegmentListBoxTypeRWTexture[location], type, output);
    InterlockedExchange(RaySegmentListEventTypeRWTexture[location], eventType, output);*/
    
    //set the ray segment list
    RaySegmentListDepthRWTexture[location] = depth;
    RaySegmentListBoxTypeRWTexture[location] = type;
    RaySegmentListEventTypeRWTexture[location] = eventType;
}

void setRaySegmentListCount(uint2 location, int value)
{
    /*int output = 0;

    InterlockedExchange(RaySegmentListCountRWTexture[location], value, output);*/

    //set the count
    RaySegmentListCountRWTexture[location] = value;
}

void addRaySegmentList(float depth, uint type, uint eventType, uint raySegmentListCount, uint2 location)
{
    uint3 firstPosition = uint3(location, raySegmentListCount - 1); //before position
    uint3 secondPosition = uint3(location, raySegmentListCount); //current position
    
    //set the event
    setRaySegmentList(depth, type, eventType, secondPosition);
    setRaySegmentListCount(location, raySegmentListCount + 1);

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
        setRaySegmentListCount(location, raySegmentListCount);

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
            setRaySegmentListCount(location, raySegmentListCount - 1);

        return;
    }

    //case 3, first is entry and second is exit
    if (beforeEventType == EntryEvent && eventType == ExitEvent) //of if (eventType == ExitEvent)
        setRaySegmentListCount(location, raySegmentListCount - 1);
}

float4 main(OutputData input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    //get pixel location
    uint2 location = uint2(input.SVPosition.xy);

    uint raySegmentListCount = RaySegmentListCountRWTexture[location];

    //get depth(distance from position to eye position)
    float depth = distance(input.Position.xyz, Setting[0].xyz);

    //find error, output
    if (raySegmentListCount + 1 >= MAX_RAYSEGMENT_COUNT)
        return float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    //front face
    if (Setting[1].x == 1 && isFrontFace == true)
        addRaySegmentList(depth, Setting[1].y, EntryEvent, raySegmentListCount, location);

    //back face
    if (Setting[1].x == 0 && isFrontFace == false)
        addRaySegmentList(depth, Setting[1].z, ExitEvent, raySegmentListCount, location);

    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}