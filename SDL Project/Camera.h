#pragma once
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

class Camera {
public:
	glm::vec3 position;
	glm::vec3 rotation;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	Camera() : position(glm::vec3(0,1.75f,0)), rotation(0), viewMatrix(1.0f) {
		projectionMatrix = glm::perspective( glm::radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f );
	}

	void Move(glm::vec3 relativeMoveVec) {
		//glm::vec3 cameraFront = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 0);
		glm::mat4 rotationMatrix = glm::rotate( glm::mat4(1.0f), rotation.x, glm::vec3(0, 1, 0) );
		glm::vec3 moveVec = rotationMatrix * glm::vec4(relativeMoveVec, 0);
		position += moveVec;
	}
	void Rotate(glm::vec3 relativeRotationVec) {
		SetRotation(rotation + relativeRotationVec);
	}

	void Update() {
		viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::rotate(viewMatrix, -rotation.y, glm::vec3(1, 0, 0));
		viewMatrix = glm::rotate(viewMatrix, -rotation.x, glm::vec3(0, 1, 0));
		viewMatrix = glm::translate(viewMatrix, -position);
	}

	void SetRotation(glm::vec3 rotation) {
		this->rotation = rotation;
		if (this->rotation.y > glm::radians(90.0))	this->rotation.y = glm::radians(90.0);
		else if (this->rotation.y < glm::radians(-90.0)) this->rotation.y = glm::radians(-90.0);
	}
};