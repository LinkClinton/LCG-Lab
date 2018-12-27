#pragma once

#include "Input.hpp"

#ifdef _WIN32

#include <Windows.h>

class WindowsInput : public Input {
private:
	RECT mOldClipRect;
public:
	WindowsInput(Framework* framework);

	virtual bool isKeyDown(KeyCode keyCode)override;

	virtual void showCursor(bool show)override;

	virtual void lockCursor()override;

	virtual void unlockCursor()override;

	virtual void setCursorPosition(const glm::vec2 &position)override;
};

#endif // _WIN32