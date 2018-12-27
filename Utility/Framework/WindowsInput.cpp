#include "WindowsInput.hpp"
#include "WindowsFramework.hpp"

#ifdef _WIN32

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


WindowsInput::WindowsInput(Framework * framework) : Input(framework)
{
	
}

bool WindowsInput::isKeyDown(KeyCode keyCode)
{
	return KEYDOWN((int)keyCode);
}

void WindowsInput::showCursor(bool show)
{
	ShowCursor(show);
}

void WindowsInput::lockCursor()
{
	GetClipCursor(&mOldClipRect);

	RECT rect; GetWindowRect((HWND)mFramework->getHandle(), &rect);

	ClipCursor(&rect);
}

void WindowsInput::unlockCursor()
{
	ClipCursor(&mOldClipRect);
}

void WindowsInput::setCursorPosition(const glm::vec2 & position)
{
	POINT point = { (int)position.x, (int)position.y };

	ClientToScreen((HWND)mFramework->getHandle(), &point);

	SetCursorPos(point.x, point.y);
}

#endif // _WIN32