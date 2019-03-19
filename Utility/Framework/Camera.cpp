#include "Camera.hpp"

Camera::Camera()
{
	mFov = 60.0f;
	mAspect = 1.3f;
	mNear = 0.01f;
	mFar = 1000.0f;

	mWidth = 1.0f;
	mHeight = 1.0f;
	
	mTransform = glm::mat4(1);
	mProjection = glm::perspective(mFov, mAspect, mNear, mFar);

	mProjectionMode = ProjectionMode::Perspective;
}

Camera::~Camera()
{
}

glm::mat4 Camera::projectionMatrix() const
{
	return mProjection;
}

glm::mat4 Camera::transformMatrix() const
{
	return mTransform;
}

glm::mat4 Camera::viewMatrix() const
{
	//view matrix is the invert of transform matrix
	//because the invert is very expensive, we need to avoid it

	//transpose it to invert the rotation of the matrix, because of orthogonal and normal
	//M * M^T = I
	auto inverse = glm::transpose(mTransform);

	inverse[3][0] = inverse[0][3] = 0.0f;
	inverse[3][1] = inverse[1][3] = 0.0f;
	inverse[3][2] = inverse[2][3] = 0.0f;

	//compute the right, up, forward vectors from matrix
	auto forward = glm::vec3(mTransform[2].x, mTransform[2].y, mTransform[2].z);
	auto right = glm::vec3(mTransform[0].x, mTransform[0].y, mTransform[0].z);
	auto up = glm::vec3(mTransform[1].x, mTransform[1].y, mTransform[1].z);

	//compute the position from matrix
	//we need to invert the translation of transform
	auto position = glm::vec3(mTransform[3].x, mTransform[3].y, mTransform[3].z);

	inverse[3][2] = -glm::dot(forward, position);
	inverse[3][0] = -glm::dot(right, position);
	inverse[3][1] = -glm::dot(up, position);

	return inverse;
}

glm::vec3 Camera::position() const
{
	return glm::vec3(mTransform[3].x, mTransform[3].y, mTransform[3].z);
}

void Camera::setTransform(const glm::mat4 & transform)
{
	mTransform = transform;

	//normalize
	if (isOrthoNormal() == false) orthoNormalize();
}

void Camera::orthoNormalize()
{
	//compute the right, up, forward vectors from matrix
	auto forward = glm::vec3(mTransform[2].x, mTransform[2].y, mTransform[2].z);
	auto right = glm::vec3(mTransform[0].x, mTransform[0].y, mTransform[0].z);
	auto up = glm::vec3(mTransform[1].x, mTransform[1].y, mTransform[1].z);

	//create new vectors with perpendicular
	auto newForward = glm::normalize(forward);
	auto newRight = glm::normalize(glm::cross(up, newForward));
	auto newUp = glm::cross(newForward, newRight);

	//rebuild the transform matrix
	mTransform = glm::mat4(
		newRight.x, newRight.y, newRight.z, 0.0f,
		newUp.x, newUp.y, newUp.z, 0.0f,
		newForward.x, newForward.y, newForward.z, 0.0f,
		mTransform[3].x, mTransform[3].y, mTransform[3].z, 1.0f);
}

void Camera::resize(int width, int height)
{
	mAspect = (float)width / height;

	if (mProjectionMode == ProjectionMode::Perspective) 
		mProjection = glm::perspective(mFov, mAspect, mNear, mFar);
	else if (mProjectionMode == ProjectionMode::Orthographic) {
		mWidth = (float)width; mHeight = (float)height;

		auto halfWidth = mWidth * 0.5f;
		auto halfHeight = mHeight * 0.5f;

		mProjection = glm::ortho(-halfWidth, halfWidth, halfHeight, -halfHeight, mNear, mFar);
	}
}

void Camera::perspective(float fov, float aspect, float near, float far)
{
	mFov = fov;
	mAspect = aspect;
	mNear = near;
	mFar = far;

	mProjection = glm::perspective(mFov, mAspect, mNear, mFar);

	mProjectionMode = ProjectionMode::Perspective;
}

void Camera::orthographic(float width, float height, float near, float far)
{
	mWidth = width; mHeight = height; mNear = near; mFar = far;

	auto halfWidth = mWidth * 0.5f;
	auto halfHeight = mHeight * 0.5f;

	mProjection = glm::ortho(-halfWidth, halfWidth, halfHeight, -halfHeight, mNear, mFar);

	mProjectionMode = ProjectionMode::Orthographic;
}

ProjectionMode Camera::projectionMode() const
{
	return mProjectionMode;
}

Frustum Camera::frustum() const
{
	assert(mProjectionMode == ProjectionMode::Perspective);

	auto viewAndPrj = projectionMatrix() * viewMatrix();

	auto column0 = glm::vec3(viewAndPrj[0].x, viewAndPrj[1].x, viewAndPrj[2].x);
	auto column1 = glm::vec3(viewAndPrj[0].y, viewAndPrj[1].y, viewAndPrj[2].y);
	auto column2 = glm::vec3(viewAndPrj[0].z, viewAndPrj[1].z, viewAndPrj[2].z);
	auto column3 = glm::vec3(viewAndPrj[0].w, viewAndPrj[1].w, viewAndPrj[2].w);
	
	auto distance = viewAndPrj[3];

	//left, top, right, bottom, near, far
	std::vector<Plane> planes(6);

	//get planes
	planes[(int)FrustumPlane::Bottom] = Plane(column3 + column1, distance.w + distance.y);
	planes[(int)FrustumPlane::Right] = Plane(column3 - column0, distance.w - distance.x);
	planes[(int)FrustumPlane::Left] = Plane(column3 + column0, distance.w + distance.x);
	planes[(int)FrustumPlane::Top] = Plane(column3 - column1, distance.w - distance.y);
	planes[(int)FrustumPlane::Far] = Plane(column3 - column2, distance.w - distance.z);
	planes[(int)FrustumPlane::Near] = Plane(column2, distance.z);

	//normalize
	for (auto &plane : planes) {
		auto length = 1.0f / glm::length(plane.normal());
		auto normal = plane.normal() * length;
		auto distance = plane.distance() * length;

		plane = Plane(normal, distance);
	}

	return Frustum(planes);
}

float Camera::aspect() const
{
	return mAspect;
}

bool Camera::isOrthographic() const
{
	return mProjectionMode == ProjectionMode::Orthographic;
}

bool Camera::isPerspective() const
{
	return mProjectionMode == ProjectionMode::Perspective;
}

bool Camera::isOrthoNormal() const
{
	//compute the right, up, forward vectors from matrix
	auto forward = glm::vec3(mTransform[2].x, mTransform[2].y, mTransform[2].z);
	auto right = glm::vec3(mTransform[0].x, mTransform[0].y, mTransform[0].z);
	auto up = glm::vec3(mTransform[1].x, mTransform[1].y, mTransform[1].z);

	//not normal vector
	if (forward.length() != 1.0f || right.length() != 1.0f || up.length() != 1.0f)
		return false;

	//not perpendicular
	if (glm::dot(forward, right) != 0.0f || 
		glm::dot(forward, up) != 0.0f || 
		glm::dot(right, up) != 0.0f)
		return false;

	return true;
}