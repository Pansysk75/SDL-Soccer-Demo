#pragma once
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Shader.h"
#include <GL\glew.h>
#include "stb_image.h"
#include "Camera.h"
#include "Light.h"
#include <vector>
#include <string>

class Model {
public:
	glm::vec3 position;
	glm::vec3 rotation;

	Mesh mesh;

	float specularAmount;
	float diffuseAmount;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO; //element buffer object -> indices
	Shader shader;
	unsigned int texture;

	Model():specularAmount(1), diffuseAmount(1), position(0), rotation(0){}

	void Render(Camera& camera, std::vector<Light>& lights) {

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.x, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
		model = glm::rotate(model, rotation.y, glm::vec3(1, 0, 0));

		glm::mat4 modelMatrix = model;
		glm::mat4 normalMatrix = glm::inverseTranspose(model);
		glm::mat4 viewMatrix = camera.projectionMatrix * camera.viewMatrix * model;;

		shader.Use();
		/////////////////////LIGHTSSS

		shader.SetAttribute( "nLights", int(lights.size()));
		for (GLuint i = 0; i < lights.size(); i++)
		{
			std::string number = std::to_string(i);
			shader.SetAttribute ("lights[" + number + "].color",lights[i].color);
			shader.SetAttribute ("lights[" + number + "].direction",lights[i].direction);
			shader.SetAttribute ("lights[" + number + "].intensity",lights[i].intensity);
		}

		////////////////////
		shader.SetAttribute("viewMatrix", viewMatrix);
		shader.SetAttribute("normalMatrix", normalMatrix);
		shader.SetAttribute("modelMatrix", normalMatrix);

		shader.SetAttribute("cameraPosition", camera.position);
		shader.SetAttribute("specularAmount", specularAmount);
		shader.SetAttribute("diffuseAmount", diffuseAmount);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size()*3);
		//glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	}



	void Load(std::string modelName) {
		mesh.Import(modelName + ".obj");
	
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//prepare raw data
		std::vector<float> vertexData;
		

		vertexData.reserve(mesh.vertices.size() * 3);
		for (int i = 0; i < mesh.vertices.size(); i++) {
			vertexData.push_back(mesh.vertices[i].x);
			vertexData.push_back(mesh.vertices[i].y);
			vertexData.push_back(mesh.vertices[i].z);
			vertexData.push_back(mesh.normals[i].x);
			vertexData.push_back(mesh.normals[i].y);
			vertexData.push_back(mesh.normals[i].z);
			vertexData.push_back(mesh.textureCoordinates[i].x);
			vertexData.push_back(mesh.textureCoordinates[i].y);
		}


		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//prepare raw data
		std::vector<unsigned int> indexData;

		indexData.reserve(mesh.indices.size());
		for (auto index : mesh.indices) {
			indexData.push_back(index);
		}


		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);


		if (std::filesystem::exists(std::filesystem::current_path().parent_path().string() + "\\Assets\\" + modelName + ".png"))
		LoadTexture(modelName + ".png");
		else if (std::filesystem::exists(std::filesystem::current_path().parent_path().string() + "\\Assets\\" + modelName + ".jpg"))
		LoadTexture(modelName + ".jpg");

	}

private:



	bool LoadTexture(std::string textureFileName) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		std::string texturePath = std::filesystem::current_path().parent_path().string() + "\\Assets\\"+textureFileName;
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data) { 
			std::cout << "Failed to load " + textureFileName +" texture" << std::endl; 
			return false;
		}
		else {
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if(nrChannels == 4)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else if (nrChannels == 3)	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//else if (nrChannels == 1)	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_R, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);


			stbi_image_free(data);
			return true;
		}
	}
};