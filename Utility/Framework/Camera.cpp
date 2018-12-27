#include "Camera.hpp"
#include "Math.hpp"

Camera::Camera(const glm::vec3 & position, const glm::mat4 & perspective)
	:mPosition(position), mPerspective(perspective)
{
	mForward = forward();
	mRight = right();
	mUp = up();
}

void Camera::walk(float length)
{
	mPosition = mPosition + mForward * length;
}

void Camera::strafe(float length)
{
	mPosition = mPosition + mRight * length;
}

void Camera::fly(float length)
{
	mPosition = mPosition + up() * length;
}

void Camera::rotateRight(float angle)
{
	auto rotate = glm::rotate(glm::tquat<float, glm::qualifier::highp>(1, 0, 0, 0), angle, mRight);

	mForward = glm::normalize(mForward * rotate);
	mUp = glm::normalize(mUp * rotate);
}

void Camera::rotateY(float angle)
{
	auto rotate = glm::rotate(glm::tquat<float, glm::qualifier::highp>(1, 0, 0, 0), angle, glm::vec3(0, 1, 0));

	mForward = glm::normalize(mForward * rotate);
	mRight = glm::normalize(mRight * rotate);
	mUp = glm::normalize(mUp * rotate);
}

void Camera::setPosition(const glm::vec3 & position)
{
	mPosition = position;
}

void Camera::setForward(const glm::vec3 & forward)
{
	//set property
	auto rotate = Math::computeRotateFromVector(mForward, forward);

	mForward = glm::normalize(mForward * rotate);
	mRight = glm::normalize(mRight * rotate);
	mUp = glm::normalize(mUp * rotate);
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

auto Camera::getView() -> glm::mat4
{
	auto center = mPosition + mForward;

	return glm::lookAt(mPosition, center, mUp);
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

auto Camera::up() -> glm::vec3
{
	return glm::vec3(0, 1, 0);
}

auto Camera::down() -> glm::vec3
{
	return glm::vec3(0, -1, 0);
}

