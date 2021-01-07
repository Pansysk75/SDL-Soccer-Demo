#include "Player.h"
#include <GL\glew.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>



void LoadAiMesh(Mesh& mesh, aiMesh* aiMesh) {
	mesh.vertices.clear();
	mesh.normals.clear();
	mesh.vertices.reserve(aiMesh->mNumVertices * 3);
	mesh.normals.reserve(aiMesh->mNumVertices * 3);

	for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
		auto aiVertex = aiMesh->mVertices[i];
		mesh.vertices.emplace_back(glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z));

		auto aiNormals = aiMesh->mNormals[i];
		mesh.normals.emplace_back(glm::vec3(aiNormals.x, aiNormals.y, aiNormals.z));

	}

	mesh.indices.clear();
	mesh.indices.reserve(aiMesh->mNumFaces * 3);

	for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
		auto aiFace = aiMesh->mFaces[i];
		for(unsigned int j=0; j<aiFace.mNumIndices; j++)
		mesh.indices.push_back(aiFace.mIndices[j]);
	}

}

void Player::LoadMesh(){
	
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile("C:/Users/George/Desktop/Panagis/Coding/SDL Project/Assets/player.dae", 
		aiProcess_Triangulate || aiProcess_PreTransformVertices 
		|| aiProcess_GenUVCoords || aiProcess_OptimizeMeshes 
		|| aiProcess_OptimizeGraph);

	if (scene == nullptr) { std::cout << "Player scene import failed:" << importer.GetErrorString() << std::endl; }
	else {

		aiMesh* aiMesh = scene->mMeshes[0];
		LoadAiMesh(this->mesh, aiMesh);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		
		//prepare raw data
		std::vector<float> vertexData;
		/*
		vertexData.reserve(mesh.vertices.size() * 3);
		for (auto vertex : mesh.vertices) {
			vertexData.push_back(vertex.x);
			vertexData.push_back(vertex.y);
			vertexData.push_back(vertex.z);
		}
		*/
		float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};


		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//prepare raw data
		std::vector<unsigned int> indexData;
		/*
		indexData.reserve(mesh.indices.size());
		for (auto index : mesh.indices) {
			indexData.push_back(index);
		}
		*/

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//shaders
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\0";

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);




		int  success;
		char infoLog[512];


		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM\n" << infoLog << std::endl;

		}

	}




}

void Player::Render() {

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout <<"OPENGL ERROR " << err << std::endl;
	}

}


