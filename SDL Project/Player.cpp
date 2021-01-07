#include "Player.h"
#include <SDL\SDL.h>
#include <GL\glew.h>
#include <glm/gtc/matrix_inverse.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

		auto aiTextureCoordinates = aiMesh->mTextureCoords[0][i];
		mesh.textureCoordinates.emplace_back(glm::vec2(aiTextureCoordinates.x, aiTextureCoordinates.y));

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
	
	const aiScene* scene = importer.ReadFile("C:/Users/George/Desktop/Panagis/Coding/SDL Project/Assets/Player.obj", 
		aiProcess_Triangulate || aiProcess_PreTransformVertices 
		|| aiProcess_OptimizeMeshes 
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
		glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		//prepare raw data
		std::vector<unsigned int> indexData;
		
		indexData.reserve(mesh.indices.size());
		for (auto index : mesh.indices) {
			indexData.push_back(index);
		}
		

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size()*sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

		//shaders
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aNorm;\n"
			"layout (location = 2) in vec2 aTexCoord;\n"
			"out vec3 Normal;\n"
			"out vec2 TexCoord;\n"
			"out vec3 vertexPos;\n"
			"uniform mat4 vertexMatrix;\n"
			"uniform mat4 normalMatrix;\n"
		

			"void main()\n"
			"{\n"
			"   gl_Position =vertexMatrix * vec4(aPos, 1.0);\n"
			"   TexCoord = aTexCoord;\n"
			"   Normal = normalize(vec3(normalMatrix*vec4(aNorm , 0.0)));\n"
			"   vertexPos = vec3(gl_Position);\n"
			"}\0";
		
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n"
			"in vec3 vertexPos;\n"

			"uniform sampler2D ourTexture;\n"
			"uniform float glossAmount;\n"
			"uniform float diffAmount;\n"
			"void main()\n"
			"{\n"
			"  vec3 redLightDir = vec3(-1, 0, 0);\n"
			"  vec3 greenLightDir = vec3(0, -1, 0);\n"
			"  vec3 blueLightDir = vec3(0, 0, -1);\n"

			"  vec4 redLightDiff = vec4(1,0,0,0) *  max(dot(Normal, -redLightDir), 0.0);\n"
			"  vec4 greenLightDiff = vec4(0,1,0,0) *max(dot(Normal, -greenLightDir), 0.0);\n"
			"  vec4 blueLightDiff = vec4(0,0,1,0) * max(dot(Normal, -blueLightDir), 0.0);\n"

			"  vec3 cameraToVertex = normalize(vertexPos - vec3(0,0,0));\n"
			"  vec4 redLightGloss = vec4(1,0,0,0) *	  max( dot(Normal, normalize(cameraToVertex - redLightDir)), 0.0 );\n"
			"  vec4 greenLightGloss = vec4(0,1,0,0) * max( dot(Normal, normalize(cameraToVertex - greenLightDir)), 0.0 );\n"
			"  vec4 blueLightGloss = vec4(0,0,1,0) *  max( dot(Normal, normalize(cameraToVertex - blueLightDir)), 0.0 );\n"
			" \n"
			//"  vec4 amb = 0.2f * texture(ourTexture, TexCoord);\n"
			//"  FragColor = texture(ourTexture, TexCoord) + amb + vec4(right, up, front, 0.0);\n"
			"vec4 diff = redLightDiff + greenLightDiff + blueLightDiff;\n"
			"vec4 gloss = redLightGloss + greenLightGloss + blueLightGloss;\n"
			//"vec4 gloss = max()\n"
			"  FragColor = diffAmount* diff + glossAmount*gloss;\n"
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


		int width, height, nrChannels;
		unsigned char* data = stbi_load("C:/Users/George/Desktop/Panagis/Coding/SDL Project/Assets/Player.png", &width, &height, &nrChannels, 0);
		if (!data) std::cout << "Failed to load Player texture" << std::endl;
		
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

		stbi_image_free(data);
	}




}

void Player::Render() {

	
	//update camera
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0, 0, -3));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotation.x, glm::vec3(0, 1, 0));
	
	glm::mat4 vertexMatrix = view * model;
	glm::mat4 normalMatrix = glm::inverseTranspose(vertexMatrix);
	vertexMatrix = projection * vertexMatrix;

	unsigned int loc_vertexMatrix = glGetUniformLocation(shaderProgram, "vertexMatrix");
	unsigned int loc_normalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	

	glUniformMatrix4fv(loc_vertexMatrix, 1, GL_FALSE, glm::value_ptr(vertexMatrix));
	glUniformMatrix4fv(loc_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	unsigned int loc_glossAmount = glGetUniformLocation(shaderProgram, "glossAmount");
	unsigned int loc_diffAmount = glGetUniformLocation(shaderProgram, "diffAmount");
	glUniform1f(loc_glossAmount, glossAmount);
	glUniform1f(loc_diffAmount, diffAmount);

	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size()*3);
	//glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout <<"OPENGL ERROR " << err << std::endl;
	}


}


void Player::Update(float dt) {
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_UP]) position += glm::vec3(0, 1, 0) * dt;
	if (keyboard[SDL_SCANCODE_DOWN]) position += glm::vec3(0, -1, 0) * dt;
	if (keyboard[SDL_SCANCODE_RIGHT]) position += glm::vec3(1, 0, 0) * dt;
	if (keyboard[SDL_SCANCODE_LEFT]) position += glm::vec3(-1, 0, 0) * dt;
	if (keyboard[SDL_SCANCODE_D]) rotation += glm::vec3(1, 0, 0) * dt;
	if (keyboard[SDL_SCANCODE_A]) rotation += glm::vec3(-1, 0, 0) * dt;

	if (keyboard[SDL_SCANCODE_PAGEUP]) { glossAmount += dt; std::cout <<"Gloss: "<< glossAmount << std::endl;}
	if (keyboard[SDL_SCANCODE_PAGEDOWN]) { glossAmount -= dt; std::cout <<"Gloss: "<< glossAmount << std::endl;}

	if (keyboard[SDL_SCANCODE_HOME]) {   diffAmount += dt; std::cout <<   "Diff: " << diffAmount << std::endl; }
	if (keyboard[SDL_SCANCODE_END]) { diffAmount -= dt; std::cout << "Diff: " << diffAmount << std::endl; }
	
}

