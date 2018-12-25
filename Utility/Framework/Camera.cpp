#include "Camera.hpp"
#include "Math.hpp"

Camera::Camera(const glm::vec3 & position, const glm::vec3 & forward, const glm::mat4 & perspective)
	:mPosition(position), mForward(forward), mPerspective(perspective)
{
	mRotate = Math::computeRotateFromVector(Camera::forward(), mForward);
}

void Camera::move(const glm::vec3 & offset)
{
	mPosition = mPosition + offset * mRotate;
}

void Camera::setPosition(const glm::vec3 & position)
{
	mPosition = position;
}

void Camera::setForward(const glm::vec3 & forward)
{
	//set property
	mRotate = Math::computeRotateFromVector(mForward, forward);
	mForward = forward;
}

void Camera::setRotate(const glm::quat & rotate)
{
	//set property
	mRotate = mRotate;
	mForward = forward() * mRotate;
}

void Camera::setPerspective(const glm::mat4 & perspective)
{
	mPerspective = perspective;
}

auto Camera::getPosition() -> glm::vec3
{
	return mPosition;
}

auto Camera::getForward() -> glm::vec3
{
	return mForward;
}

auto Camera::getRotate() -> glm::quat
{
	return mRotate;
}

auto Camera::getView() -> glm::mat4
{
	auto up = glm::vec3(0, 1, 0) * mRotate;
	auto center = mPosition + mForward;

	return glm::lookAt(mPosition, center, up);
}

auto Camera::getPerspective() -> glm::mat4
{
	return mPerspective;
}

auto Camera::forward() -> glm::vec3
{
	return glm::vec3(0, 0, 1);
}

auto Camera::back() -> glm::vec3
{
	return glm::vec3(0, 0, -1);
}

auto Camera::left() -> glm::vec3
{
	return glm::vec3(1, 0, 0);
}

auto Camera::right() -> glm::vec3
{
	return glm::vec3(-1, 0, 0);
}

