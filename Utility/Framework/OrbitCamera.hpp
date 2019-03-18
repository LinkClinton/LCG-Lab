#pragma once

#include "Camera.hpp"

class OrbitCamera : public Camera {
protected:
	glm::vec2 mCurrentRotation;
	glm::vec2 mRotateSpeed;
	glm::vec2 mPanSpeed;


	glm::vec3 mTarget;
	
	float mZoomDistanceMinLimit;
	float mZoomDistanceMaxLimit;
	float mZoomDistance;
	float mZoomSpeed;

	static float clampAngle(float angle, float min, float max);
public:
	OrbitCamera(const glm::vec3 &target = glm::vec3(0, 0, 0), float distance = 10.0f);

	~OrbitCamera();

	void setRotateSpeed(const glm::vec2 &rotateSpeed);
	void setPanSpeed(const glm::vec2 &panSpeed);
	void setZoomSpeed(float zoomSpeed);

	void setZoomLimit(float minLimit, float maxLimit);
	
	void rotate(const glm::vec2 &deltaRotation, float deltaTime);
	void pan(const glm::vec2 &deltaPan, float deltaTime);
	void zoom(float deltaZoom, float deltaTime);

	void update(float deltaTime);
};