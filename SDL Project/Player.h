#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Skybox.h"

struct Mesh {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
};


class Player
{	
	glm::vec3 position;
	glm::vec3 rotation;

	glm::vec3 cameraPosition;
	glm::vec3 cameraRotation;
	glm::mat4x4 cameraProjection;
	
	Mesh mesh;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO; //element buffer object -> indices
	unsigned int texture;

	Skybox skybox;

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;

	float specularAmount;
	float diffuseAmount;


	//texture
	std::vector<float> textureCoordinates;

public:
	void LoadMesh();
	void Update(float dt);
	void Render();
	
};





