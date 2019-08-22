#pragma pack_matrix(row_major) 

cbuffer Transform : register(b0) {
	matrix world;
	matrix project;
}

StructuredBuffer<matrix> InstanceBuffer : register(t0);

RWTexture2D<float> BufferTexture2D : register(u1);
RWTexture2D<uint>  CountTexture2D : register(u2);
RWTexture2D<float> HeatMapTexture2D : register(u3);