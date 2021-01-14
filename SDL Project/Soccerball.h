#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "Model.h"
#include "ResourceManager.h"

class Soccerball {

public:
	glm::vec3 position;
	glm::vec3 rotation;
	float radius;

	float mass;
	glm::vec3 velocity;

	Model model;



	void Load() {
		model.SetShader(ResourceManager::GetShader());
		model.Load("soccerball");
		model.specularAmount = 0.1f;
		model.diffuseAmount = 1.0f;
		position = glm::vec3(0, 2, 0);
		rotation = glm::vec3(0, 0, 0);

		velocity = glm::vec3(0, 0, 0);
		mass = 0.43f;
		radius = 0.4f;
	
	}
	void Update(float dt) {
		position += velocity * dt;
		rotation.y += 0.005f * velocity.z;
		rotation.z += -0.005f * velocity.x;

		model.position = position;
		model.rotation = rotation;
	}
	void Render(Camera& camera) {
		model.Render(camera);
	}
};