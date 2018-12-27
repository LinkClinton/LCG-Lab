#pragma once

#include <glm\glm.hpp>

#include "KeyCode.hpp"
#include "MouseButton.hpp"

class BaseEvent {

};

class KeyBoardEvent : public BaseEvent {
private:
	bool mKeyDown;
	KeyCode mKeyCode;
public:
	KeyBoardEvent(bool keyDown, KeyCode keyCode): 
		mKeyDown(keyDown), mKeyCode(keyCode) {}

	bool isKeyDown();

	auto getKeyCode() -> KeyCode;
};

class MouseMoveEvent : public BaseEvent {
private:
	glm::vec2 mPosition;
public:
	MouseMoveEvent(const glm::vec2 &position) : mPosition(position) {}

	auto getPosition() -> glm::vec2;
};

class MouseClickEvent : public MouseMoveEvent {
private:
	MouseButton mButton;
	bool mIsDown;
public:
	MouseClickEvent(const glm::vec2 &position, MouseButton button, bool isDown) :
		MouseMoveEvent(position), mButton(button), mIsDown(isDown) {}

	auto getMouseButton() -> MouseButton;

	auto isDown() -> bool;
};