#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "Model.h"

#include "Skybox.h"




class Player
{	
	glm::vec3 position;
	glm::vec3 rotation;

	glm::vec3 cameraPosition;
	glm::vec3 cameraRotation;
	glm::mat4x4 cameraProjection;
	
	Model model;
	Skybox skybox;

	float specularAmount;
	float diffuseAmount;


public:
	void Load();
	void Update(float dt);
	void Render();
	
};





