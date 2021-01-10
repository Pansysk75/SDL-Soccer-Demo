#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "Model.h"

class Field
{
	glm::vec3 position;
	glm::vec3 rotation;

	Model model;
	Mesh collisionMesh;


public:
	void Load() {
		model.Load("field\\field");
		model.specularAmount = 0.1f;
		model.diffuseAmount = 1.0f;
		position = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		collisionMesh.Import("Player_CollisionMesh");
	}
	void Update(float dt);
	void Render(Camera& camera, std::vector<Light>& lights) {
		model.Render(camera, lights);
	}
};

