#pragma pack_matrix(row_major)

struct sharp_instance_buffer {
	matrix world;
	float4 color;
};

struct output {
	float4 position : SV_POSITION;
	uint id : SV_INSTANCEID;
};

cbuffer Transform : register(b0) {
	matrix world;
	matrix project;
}

StructuredBuffer<sharp_instance_buffer> sharp_buffer : register(t0);