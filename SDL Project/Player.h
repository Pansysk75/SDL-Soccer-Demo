#pragma once
#include <glm/vec3.hpp>
#include <vector>


struct Mesh {
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
};


class Player
{
	
	glm::vec3 position;
	
	Mesh mesh;

	//texture
	std::vector<float> textureCoordinates;

public:
	void LoadMesh();
	void Render();
	
};





