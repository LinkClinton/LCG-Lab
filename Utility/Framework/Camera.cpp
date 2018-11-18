#include "Camera.hpp"

void Camera::setPosition(const glm::vec3 & position)
{
	mPosition = position;
}

void Camera::setLookAt(const glm::vec3 & lookAt)
{
	mLookAt = lookAt;
}

void Camera::setUp(const glm::vec3 & up)
{
	mUp = up;
}

auto Camera::getPosition() -> glm::vec3 const
{
	return mPosition;
}

auto Camera::getLookAt() -> glm::vec3 const
{
	return mLookAt;
}

auto Camera::getUp() -> glm::vec3 const
{
	return mUp;
}

auto Camera::getMatrix() -> glm::mat4 const
{
	return glm::lookAt(mPosition, mLookAt, mUp);
}
