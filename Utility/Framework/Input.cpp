#include "Input.hpp"

#ifdef _WIN32
#include <Windows.h>

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#endif // _WIN32


bool Input::isKeyDown(KeyCode keyCode)
{
#ifdef _WIN32
	return KEYDOWN((int)keyCode);
#endif // _WIN32
}
