#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "Model.h"


class Goalpost {

	glm::vec3 position;
	glm::vec3 rotation;

	Model goalpostModel;
	Model netModel;
	Mesh collisionMesh;


public:
	void Load() {
		goalpostModel.Load("field\\goalpost");
		goalpostModel.specularAmount = 0.0f;
		goalpostModel.diffuseAmount = 1.0f;
		netModel.Load("field\\goalpost_net");
		netModel.specularAmount = 0.0f;
		netModel.diffuseAmount = 1.0f; 
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		collisionMesh.Import("Player_CollisionMesh");
	}
	void Update(float dt);
	void Render(Camera& camera, std::vector<Light>& lights) {
		goalpostModel.Render(camera, lights);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		netModel.Render(camera, lights);
		glDisable(GL_BLEND);
	}

	void SetPosition(glm::vec3 position) { 
		this->position = position; 
		goalpostModel.position = position;
		netModel.position = position;
	}

	void SetRotation(glm::vec3 rotation) {
		this->rotation = rotation;
		goalpostModel.rotation = rotation;
		netModel.rotation = rotation;
	}
};