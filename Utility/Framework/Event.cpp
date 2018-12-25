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
