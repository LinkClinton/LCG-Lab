#pragma once

#include <string>

#include "Utility.hpp"

#include "Factory.hpp"
#include "Graphics.hpp"
#include "SwapChain.hpp"

class Framework {
protected:
	std::string mName;

	int mWidth;
	int mHeight;

	bool mIsWindowExist;

	Factory* mFactory;
	Graphics* mGraphics;
	SwapChain* mSwapChain;

	virtual void update(void* sender) = 0;

	virtual void render(void* sender) = 0;

	Framework(const std::string &name, int width, int height) :
		mName(name), mWidth(width), mHeight(height), mIsWindowExist(false),
		mGraphics(nullptr), mSwapChain(nullptr) {}

public:
	virtual void showWindow() = 0;

	virtual void hideWindow() = 0;

	virtual void runLoop() = 0;
};