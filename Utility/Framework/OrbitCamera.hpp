#pragma once

#include "Camera.hpp"

class OrbitCamera : public Camera {
protected:
	glm::vec2 mCurrentRotation;
	glm::vec2 mYRotateLimit;
	glm::vec2 mRotateSpeed;
	glm::vec2 mPanSpeed;


	glm::vec3 mTarget;
	
	float mZoomDistanceMinLimit;
	float mZoomDistanceMaxLimit;
	float mZoomDistance;
	float mZoomSpeed;

	static float clampAngle(float angle, float min, float max);
public:
	OrbitCamera();

	~OrbitCamera();

	void rotate(const glm::vec2 &deltaRotation, float deltaTime);
	void pan(const glm::vec2 &deltaPan, float deltaTime);
	void zoom(float deltaZoom, float deltaTime);

	void update(float deltaTime);
};