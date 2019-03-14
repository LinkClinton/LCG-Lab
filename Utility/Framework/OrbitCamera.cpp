#include "OrbitCamera.hpp"
#include "Utility.hpp"

#include <iostream>

float OrbitCamera::clampAngle(float angle, float min, float max)
{
	auto twoPi = glm::two_pi<float>();

	while (angle < -twoPi) angle = angle + twoPi;
	while (angle >  twoPi) angle = angle - twoPi;

	return Utility::clamp(angle, min, max);
}

OrbitCamera::OrbitCamera()
{
	mTarget = glm::vec3(0, 0, 0);
	mZoomDistance = 10.0f;
	mZoomSpeed = 10.0f;
	mRotateSpeed = glm::vec2(10.0f / 180.0f, 10.0f / 180.0f);
	mCurrentRotation = glm::vec2(0, 0);
	mPanSpeed = glm::vec2(1.0f, 1.0f);

	mZoomDistanceMinLimit = 3.0f;
	mZoomDistanceMaxLimit = 15.0f;
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::rotate(const glm::vec2 & deltaRotation, float deltaTime)
{
	mCurrentRotation.x = mCurrentRotation.x + deltaRotation.x * mRotateSpeed.x * mZoomDistance * deltaTime;
	mCurrentRotation.y = mCurrentRotation.y + deltaRotation.y * mRotateSpeed.y * mZoomDistance * deltaTime;

	mCurrentRotation.x = clampAngle(mCurrentRotation.x, -glm::two_pi<float>(), glm::two_pi<float>());
	mCurrentRotation.y = clampAngle(mCurrentRotation.y, -glm::two_pi<float>(), glm::two_pi<float>());
}

void OrbitCamera::pan(const glm::vec2 & deltaPan, float deltaTime)
{
	auto right = glm::vec3(mTransform[0].x, mTransform[0].y, mTransform[0].z);

	auto xPanMag = deltaPan.x * mPanSpeed.x * deltaTime;
	auto yPanMag = deltaPan.y * mPanSpeed.y * deltaTime;

	mTarget = mTarget - (right * xPanMag);
	mTarget = mTarget + (glm::vec3(0, 1, 0) * yPanMag);
}

void OrbitCamera::zoom(float deltaZoom, float deltaTime)
{
	mZoomDistance = mZoomDistance + deltaZoom * mZoomSpeed * deltaTime;
	mZoomDistance = Utility::clamp(mZoomDistance, mZoomDistanceMinLimit, mZoomDistanceMaxLimit);
}

void OrbitCamera::update(float deltaTime)
{
	auto rotationMatrix = glm::mat4(1);
	
	//error way for rotation
	rotationMatrix = glm::rotate(rotationMatrix, mCurrentRotation.x, glm::vec3(0, 1, 0));
	rotationMatrix = glm::rotate(rotationMatrix, mCurrentRotation.y, glm::vec3(1, 0, 0));

	auto direction = glm::vec4(0.0f, 0.0f, mZoomDistance, 1.0f) * rotationMatrix;
	auto position = mTarget - glm::vec3(direction);

	auto up = glm::vec3(0, 1, 0);

	//change up
	if (mCurrentRotation.y >  glm::half_pi<float>()) up = glm::vec3(0, -1, 0);
	if (mCurrentRotation.y < -glm::half_pi<float>()) up = glm::vec3(0, -1, 0);
	if (mCurrentRotation.y >  glm::half_pi<float>() + glm::pi<float>()) up = glm::vec3(0, 1, 0);
	if (mCurrentRotation.y < -glm::half_pi<float>() - glm::pi<float>()) up = glm::vec3(0, 1, 0);

	mTransform = glm::inverse(glm::lookAt(position, mTarget, up));

}
