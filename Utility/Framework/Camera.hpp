#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>


class Camera {
private:
	glm::vec3 mPosition;
	glm::vec3 mForward;
	glm::vec3 mRight;
	glm::vec3 mUp;

	glm::mat4 mPerspective;
public:
	Camera(
		const glm::vec3 &position = glm::vec3(0, 0, 0),
		const glm::mat4 &perspective = glm::mat4(1));

	void walk(float length);

	void strafe(float length);

	void rotateRight(float angle);

	void rotateY(float angle);

	void setPosition(const glm::vec3 &position);
	
	void setForward(const glm::vec3 &forward);

	void setPerspective(const glm::mat4 &perspective);

	auto getPosition() -> glm::vec3;

	auto getForward() -> glm::vec3;

	auto getView() -> glm::mat4;

	auto getPerspective() -> glm::mat4;

	static auto forward() -> glm::vec3;

	static auto back() -> glm::vec3;

	static auto left() -> glm::vec3;

	static auto right() -> glm::vec3;

	static auto up() -> glm::vec3;

	static auto down() -> glm::vec3;
};
