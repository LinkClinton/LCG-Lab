#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>


class Camera {
private:
	glm::vec3 mPosition;
	glm::vec3 mForward;
	
	glm::quat mRotate;

	glm::mat4 mPerspective;
public:
	Camera(
		const glm::vec3 &position = glm::vec3(0, 0, 0), 
		const glm::vec3 &forward = forward(),
		const glm::mat4 &perspective = glm::mat4(1));

	void move(const glm::vec3 &offset);

	void setPosition(const glm::vec3 &position);
	
	void setForward(const glm::vec3 &forward);

	void setRotate(const glm::quat &rotate);

	void setPerspective(const glm::mat4 &perspective);

	auto getPosition() -> glm::vec3;

	auto getForward() -> glm::vec3;

	auto getRotate() -> glm::quat;

	auto getView() -> glm::mat4;

	auto getPerspective() -> glm::mat4;

	static auto forward() -> glm::vec3;

	static auto back() -> glm::vec3;

	static auto left() -> glm::vec3;

	static auto right() -> glm::vec3;
};
