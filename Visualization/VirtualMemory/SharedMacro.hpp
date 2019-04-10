#ifndef __SHAREDMACRO_H__
#define __SHAREDMACRO_H__

//some const definition 
//it was shared with (C++ and HLSL)

/**
 * \brief enable sparse leap mode
 */
#define _SPARSE_LEAP

/**
 * \brief 
 * the block size of x-axis,y-axis and z-axis \n
 * the texture size(xyz-axis) is equal BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ
 */
#define BLOCK_SIZE_XYZ 32

/**
 * \brief 
 * the block count of x-axis, y-axis and z-axis \n
 * the texture size(xyz-axis) is equal BLOCK_COUNT_XYZ * BLOCK_SIZE_XYZ
 */
#define BLOCK_COUNT_XYZ 16

/**
 * \brief the max cache miss count of ray in per frame
 */
#define BLOCK_RAY_CACHE_MISS_MAX_PER_FRAME 4

/**
 * \brief 
 * the size of pixel tile shared same hash table \n
 * it is used for cache miss to hash block id
 */
#define BLOCK_HASH_TABLE_PIXEL_TILE_SIZE_XY 64

/**
 * \brief the size of hash table, it is used for cache miss to hash block id
 */
#define BLOCK_HASH_TABLE_ARRAY_SIZE 32

/**
 * \brief 
 * the page block size of x-axis, y-axis and z-axis \n
 * the texture size is equal PAGE_SIZE_XYZ * PAGE_SIZE_COUNT
 */
#define PAGE_SIZE_XYZ 8

/**
 * \brief 
 * the page block count of x-axis, y-axis and z-axis \n
 * the texture size is equal PAGE_SIZE_XYZ * PAGE_SIZE_COUNT
 */
#define PAGE_COUNT_XYZ 16

/**
 * \brief the max multi-resolution count(in other words, the directory count)
 */
#define MAX_MULTIRESOLUTION_COUNT 8

/**
 * \brief max buffer size of read
 */
#define MAX_READ_BUFFER 16384

/**
 * \brief the max count of ray segment
 */
#define MAX_RAYSEGMENT_COUNT 30

/**
 * \brief max depth of OccupancyHistogramTree
 */
#define MAX_DEPTH 5

/**
 * \brief max value that we can think it is empty
 */
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