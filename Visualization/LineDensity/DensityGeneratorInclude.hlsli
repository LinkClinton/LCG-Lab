#pragma pack_matrix(row_major) 

struct instance_data {
	matrix world;
};

cbuffer Transform : register(b0) {
	matrix world;
	matrix project;
}

StructuredBuffer<instance_data> InstanceBuffer : register(t0);

RWTexture2D<float> BufferTexture2D : register(u1);
RWTexture2D<uint>  CountTexture2D : register(u2);
RWTexture2D<float> HeatMapTexture2D : register(u3);