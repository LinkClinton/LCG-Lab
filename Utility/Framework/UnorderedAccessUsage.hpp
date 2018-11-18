#pragma once

#include "PixelFormat.hpp"

class Texture2D;
class Texture3D;
class Graphics;

class UnorderedAccessUsage {
protected:
	Graphics* mGraphics;

	PixelFormat mPixelFormat;

	UnorderedAccessUsage(Graphics* graphics, Texture2D* texture2D, PixelFormat pixelFormat) :
		mGraphics(graphics), mPixelFormat(pixelFormat) {}

	UnorderedAccessUsage(Graphics* graphics, Texture3D* texture3D, PixelFormat pixelFormat) :
		mGraphics(graphics), mPixelFormat(pixelFormat) {}
};