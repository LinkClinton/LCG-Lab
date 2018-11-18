#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera {
private:
	glm::vec3 mPosition;
	glm::vec3 mLookAt;
	glm::vec3 mUp;
public:
	void setPosition(const glm::vec3 &position);

	void setLookAt(const glm::vec3 &lookAt);

	void setUp(const glm::vec3 &up);

	auto getPosition()->glm::vec3 const;

	auto getLookAt()->glm::vec3 const;

	auto getUp()->glm::vec3 const;

	auto getMatrix()->glm::mat4 const;
};