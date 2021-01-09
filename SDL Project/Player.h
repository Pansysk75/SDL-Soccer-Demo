#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"




class Player
{	
	glm::vec3 position;
	glm::vec3 rotation;

	Camera camera;
	glm::mat4x4 cameraProjection;
	
	Model model;
	Skybox skybox;


public:
	void Load();
	void Update(float dt);
	void Render();
	
};





