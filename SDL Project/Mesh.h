#pragma once
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <filesystem>
#include <string>

struct Mesh {

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;


	void Import(std::string meshFileName) {
		std::string modelPath = std::filesystem::current_path().parent_path().string() + "\\Assets\\" + meshFileName;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(modelPath,
			aiProcess_Triangulate || aiProcess_PreTransformVertices
			|| aiProcess_OptimizeMeshes
			|| aiProcess_OptimizeGraph);

		if (scene == nullptr) { std::cout << "Player scene import failed:" << importer.GetErrorString() << std::endl; }
		else {

			aiMesh* aiMesh = scene->mMeshes[0];
		
			Mesh& mesh = *this;
			mesh.vertices.clear();
			mesh.normals.clear();
			mesh.vertices.reserve(aiMesh->mNumVertices * 3);
			mesh.normals.reserve(aiMesh->mNumVertices * 3);

			for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
				auto aiVertex = aiMesh->mVertices[i];
				mesh.vertices.emplace_back(glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z));

				auto aiNormals = aiMesh->mNormals[i];
				mesh.normals.emplace_back(glm::vec3(aiNormals.x, aiNormals.y, aiNormals.z));

				auto aiTextureCoordinates = aiMesh->mTextureCoords[0][i];
				mesh.textureCoordinates.emplace_back(glm::vec2(aiTextureCoordinates.x, aiTextureCoordinates.y));

			}

			mesh.indices.clear();
			mesh.indices.reserve(aiMesh->mNumFaces * 3);

			for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
				auto aiFace = aiMesh->mFaces[i];
				for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
					mesh.indices.push_back(aiFace.mIndices[j]);
			}
		}
	}
};