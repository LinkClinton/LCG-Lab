#pragma once

#include <Framework.hpp>

class SharedTexture3D {
private:
	Texture3D* mGpuTexture;
	Texture3D* mCpuTexture;

	Factory* mFactory;
public:
	SharedTexture3D(int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info,
		Factory* factory);

	~SharedTexture3D();

	void update();

	auto mapCpuTexture() -> byte*;

	auto unmapCpuTexture();

	auto getWidth() -> int;

	auto getHeight() -> int;

	auto getDepth() -> int;

	auto getPixelFormat() -> PixelFormat;

	auto getGpuTexture() -> Texture3D*;

	auto getCpuTexture() -> Texture3D*;
};