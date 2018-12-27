#pragma once

#include <glm/glm.hpp>

#include "KeyCode.hpp"

class Framework;

class Input {
protected:
	Framework* mFramework;
public:
	Input(Framework* framework) : mFramework(framework) {};

	virtual bool isKeyDown(KeyCode keyCode) = 0;

	virtual void showCursor(bool show) = 0;

	virtual void lockCursor() = 0;

	virtual void unlockCursor() = 0;

	virtual void setCursorPosition(const glm::vec2 &position) = 0;
};