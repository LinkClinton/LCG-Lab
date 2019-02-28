#pragma once

struct MappedData {
	void* Data;
	int RowPitch;
	int DepthPitch;

	MappedData(void* pointer = nullptr, int rowPitch = 0, int depthPitch = 0) :
		Data(pointer), RowPitch(rowPitch), DepthPitch(depthPitch) {}
};