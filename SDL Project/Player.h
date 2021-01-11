#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Soccerball.h"



class Player
{	
	glm::vec3 position;
	glm::vec3 rotation;
	
	glm::vec3 velocity;
	bool canJump;

	Camera camera;
	
	Model model;
	Mesh collisionMesh;

	//Player attributes:
	float runSpeed;
	float sprintSpeed;
	float friction;

	bool hasBall;

	float shootPowerBuildup;
	float shootingPower;
	float shootingLift;

public:
	Soccerball ball;
	void Load();
	void Update(float dt);
	void Render(std::vector<Light>& lights, std::vector<Light_Point>& pointLights);
	Camera& GetCamera() { return camera; }
	
};





