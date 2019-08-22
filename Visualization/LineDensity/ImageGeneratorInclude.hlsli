#include "SharedMacro.hpp"

#pragma pack_matrix(row_major) 

cbuffer UploadData : register(b0) {
	matrix world;
	matrix project;
	matrix others;
}

cbuffer ColorMapped : register(b1) {
	float4 colors[MAX_COLOR_POINTS];
}

RWTexture2D<float> HeatMapTexture2D : register(u1);