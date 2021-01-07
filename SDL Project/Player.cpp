#include "Player.h"
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
		mesh.vertices.push_back(aiVertex.x);
		mesh.vertices.push_back(aiVertex.y);
		mesh.vertices.push_back(aiVertex.z);

		auto aiNormals = aiMesh->mNormals[i];
		mesh.normals.push_back(aiNormals.x);
		mesh.normals.push_back(aiNormals.y);
		mesh.normals.push_back(aiNormals.z);
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


	}
}


