#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>

enum class ProjectionMode {
	Perspective,
	Orthographic
};

class Camera {
protected:
	float mFov;
	float mAspect;
	float mNear;
	float mFar;

	float mWidth;
	float mHeight;

	glm::mat4 mTransform;
	glm::mat4 mProjection;

	ProjectionMode mProjectionMode;
public:
	Camera();

	~Camera();

	void setTransform(const glm::mat4 &transform);
	void orthoNormalize();
	void resize(int width, int height);
	void perspective(float fov, float aspect, float near = 0.01f, float far = 1000.0f);
	void orthographic(float width, float height, float near = 0.01f, float far = 1000.0f);

	ProjectionMode projectionMode() const;

	glm::mat4 projectionMatrix() const;
	glm::mat4 transformMatrix() const;
	glm::mat4 viewMatrix();
	glm::vec3 position() const;

	float aspect() const;
	
	bool isOrthographic() const;
	bool isPerspective() const;
	bool isOrthoNormal() const;
};