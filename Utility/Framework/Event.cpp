#include "Event.hpp"

bool KeyBoardEvent::isKeyDown()
{
	return mKeyDown;
}

auto KeyBoardEvent::getKeyCode() -> KeyCode
{
	return mKeyCode;
}

auto MouseMoveEvent::getPosition() -> glm::vec2
{
	return mPosition;
}

auto MouseClickEvent::getMouseButton() -> MouseButton
{
	return mButton;
}

auto MouseClickEvent::isDown() -> bool
{
	return mIsDown;
}

