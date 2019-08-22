#pragma once

#include "PixelFormat.hpp"

class Texture2D;
class Texture3D;
class StructuredBuffer;
class Graphics;

class ResourceUsage {
protected:
	Graphics* mGraphics;

	PixelFormat mPixelFormat;

	ResourceUsage(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat) :
		mGraphics(graphics), mPixelFormat(pixelFormat) {}

	ResourceUsage(Graphics* graphics, Texture3D* texture3D, PixelFormat pixelFormat) :
		mGraphics(graphics), mPixelFormat(pixelFormat) {}

	ResourceUsage(Graphics* graphics, StructuredBuffer* buffer) :
		mGraphics(graphics), mPixelFormat(PixelFormat::Unknown) {}
};
