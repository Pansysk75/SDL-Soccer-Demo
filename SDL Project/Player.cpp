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
		vertexData.reserve(mesh.vertices.size() * 3);
		for (auto vertex : mesh.vertices) {
			vertexData.push_back(vertex.x);
			vertexData.push_back(vertex.y);
			vertexData.push_back(vertex.z);
		}

		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	}
}

void Player::Render() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
}


