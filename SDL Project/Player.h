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
	Skybox skybox;
	Soccerball ball;


public:
	void Load();
	void Update(float dt);
	void Render();
	Camera& GetCamera() { return camera; }
	
};





