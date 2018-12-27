#pragma once

#include <string>

#include "Utility.hpp"

#include "Factory.hpp"
#include "Graphics.hpp"
#include "SwapChain.hpp"
#include "Event.hpp"

class Framework {
protected:
	std::string mName;

	int mWidth;
	int mHeight;

	bool mIsWindowExist;

	Factory* mFactory;
	Graphics* mGraphics;
	SwapChain* mSwapChain;

	virtual void update(void* sender, float deltaTime) = 0;

	virtual void render(void* sender, float deltaTime) = 0;

	virtual void keyUp(void* sender, KeyBoardEvent* eventArg) = 0;

	virtual void keyDown(void* sender, KeyBoardEvent* eventArg) = 0;

	virtual void mouseMove(void* sender, MouseMoveEvent* eventArg) = 0;

	virtual void mouseUp(void* sender, MouseClickEvent* eventArg) = 0;

	virtual void mouseDown(void* sender, MouseClickEvent* eventArg) = 0;

	Framework(const std::string &name, int width, int height) :
		mName(name), mWidth(width), mHeight(height), mIsWindowExist(false),
		mGraphics(nullptr), mSwapChain(nullptr) {}

public:
	virtual void showWindow() = 0;

	virtual void hideWindow() = 0;

	virtual void runLoop() = 0;

	virtual auto getDeltaTime() -> float = 0;

	virtual auto getHandle() -> void* = 0;
};

#include "Input.hpp"