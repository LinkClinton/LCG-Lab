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

	void update() const;

	auto mapCpuTexture() const -> MappedData;

	void unmapCpuTexture() const;

	auto getWidth() const -> int;

	auto getHeight() const -> int;

	auto getDepth() const -> int;

	auto getPixelFormat() const -> PixelFormat;

	auto getGpuTexture() const -> Texture3D*;

	auto getCpuTexture() const -> Texture3D*;
};