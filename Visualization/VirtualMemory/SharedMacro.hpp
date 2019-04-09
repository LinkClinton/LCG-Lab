#ifndef __SHAREDMACRO_H__
#define __SHAREDMACRO_H__

//some const definition 
//it was shared with (C++ and HLSL)

//enable sparse leap
#define _SPARSE_LEAP

//BLOCK_SIZE_XYZ : 
//the block size of x-axis,y-axis and z-axis
//the texture size(xyz-axis) is euqal BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ
#define BLOCK_SIZE_XYZ 32
//BLOCK_COUNT_XYZ :
//the block count of x-axis, y-axis and z-axis
//the texture size(xyz-axis) is euqal BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ
#define BLOCK_COUNT_XYZ 16
//BLOCK_RAY_CACHE_MISS_MAX_PER_FRAME :
//the max cache miss count of ray in per frame
#define BLOCK_RAY_CACHE_MISS_MAX_PER_FRAME 4
//BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY :
//the size of pixel tile shared same hash table
//it is used for cache miss to hash block id
#define BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY 64
//BLOCK_HASH_TABLE_ARRAY_SIZE :
//the size of hash table, it is used for cache miss to hash block id
#define BLOCK_HASH_TABLE_ARRAY_SIZE 32

//PAGE_SIZE_XYZ :
//the page block size of x-axis, y-axis and z-axis
//the texture size is euqal PAGE_SIZE_XYZ * PAGE_SIZE_COUNT
#define PAGE_SIZE_XYZ 8
//PAGE_COUNT_XYZ :
//the page block count of x-axis, y-axis and z-axis
//the texture size is euqal PAGE_SIZE_XYZ * PAGE_SIZE_COUNT
#define PAGE_COUNT_XYZ 16

//MAX_MULTIRESOLUTION_COUNT
//the max multi-resolution count(in other words, the directory count)
#define MAX_MULTIRESOLUTION_COUNT 8

//max buffer size of read
#define MAX_READ_BUFFER 16384

//for sparse leap
//the max count of ray segment
#define MAX_RAYSEGMENT_COUNT 30

//max depth of OccupancyHistogramTree
#define MAX_DEPTH 5

//max value that we can think it is empty
#define EMPTY_LIMIT 0.145

//for HLSL
#ifndef __cplusplus

//Page State for HLSL
#define MAPPED 1
#define UNMAPPED 0
#define EMPTY 2

#define BOX_TYPE_EMPTY 0
#define BOX_TYPE_NO_EMPTY 1
#define BOX_TYPE_UNKNOWN 2

#endif // !__cplusplus


#endif 