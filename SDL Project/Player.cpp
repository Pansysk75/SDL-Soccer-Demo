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
#include <filesystem>
#include <string>



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
	
	std::string modelPath = std::filesystem::current_path().parent_path().string() + "\\Assets\\Player.obj";

	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(modelPath, 
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
		glEnableVertexAttribArray(2);

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
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 normalMatrix;\n"
			"uniform mat4 modelMatrix;\n"
		

			"void main()\n"
			"{\n"
			"   gl_Position =viewMatrix * vec4(aPos, 1.0);\n"
			"   TexCoord = aTexCoord;\n"
			"   Normal = normalize(vec3(normalMatrix*vec4(aNorm , 0.0)));\n"
			"   vertexPos = vec3( modelMatrix * vec4(aPos, 1.0) );\n"
			"}\0";
		
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n"
			"in vec3 vertexPos;\n"
			"uniform vec3 cameraPosition;\n"

			"uniform sampler2D ourTexture;\n"
			"uniform float specularAmount;\n"
			"uniform float diffuseAmount;\n"
			"void main()\n"
			"{\n"
			"  vec3 L1Dir = normalize(vec3(1, 0, 0.0));\n"
			"  vec3 L2Dir = normalize(vec3(-1, -0.7, -1));\n"
			"  vec3 L3Dir = vec3(0, 0, -1);\n"

			"  vec4 L1Color = vec4(0.0, 0.4, 0.4, 1);\n"
			"  vec4 L2Color = vec4(1, 1, 1, 1);\n"
			"  vec4 L3Color = vec4(0, 0, 0, 1);\n"

			"  vec4 L1Diffuse = L1Color *  max(dot(Normal, -L1Dir), 0.0);\n"
			"  vec4 L2Diffuse = L2Color *  max(dot(Normal, -L2Dir), 0.0);\n"
			"  vec4 L3Diffuse = L3Color *  max(dot(Normal, -L3Dir), 0.0);\n"

			//"  vec3 viewDir = normalize(cameraPosition - vertexPos);\n"
			"  vec3 viewDir = normalize( cameraPosition - vertexPos);\n"

			"  vec4 L1Specular = L1Color * pow(	max( dot(viewDir, reflect(L1Dir, Normal)), 0.0 ), 8);\n"
			"  vec4 L2Specular = L2Color * pow( max( dot(viewDir, reflect(L2Dir, Normal)), 0.0 ), 8);\n"
			"  vec4 L3Specular = L3Color * pow( max( dot(viewDir, reflect(L3Dir, Normal)), 0.0 ), 8);\n"
			" \n"
			//"  vec4 amb = 0.2f * texture(ourTexture, TexCoord);\n"

			"vec4 diffuse = texture(ourTexture, TexCoord) * (L1Diffuse + L2Diffuse + L3Diffuse);\n"
			"vec4 specular = L1Specular + L2Specular + L3Specular;\n"

			"  FragColor = diffuseAmount* diffuse + specularAmount*specular;\n"
			//"  FragColor =texture(ourTexture, TexCoord);\n "
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

		//PLAYER TEXTURE
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		std::string texturePath = std::filesystem::current_path().parent_path().string() + "\\Assets\\Player.png";
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data) std::cout << "Failed to load Player texture" << std::endl;
		else {
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


		skybox.Load();

		

	}




}

void Player::Render() {

	
	//update camera
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::rotate(view, -cameraRotation.x, glm::vec3(0,1,0));
	view = glm::rotate(view, -cameraRotation.y, glm::vec3(1,0,0));
	view = glm::translate(view,-cameraPosition);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotation.x, glm::vec3(0, 1, 0));
	
	glm::mat4 modelMatrix = model;
	glm::mat4 normalMatrix = glm::inverseTranspose(model);
	glm::mat4 viewMatrix = projection * view * model;;

	skybox.Render(projection * glm::mat4(glm::mat3(view)) );
	//skybox.Render(view  * projection);

	glUseProgram(shaderProgram);
	unsigned int loc_viewMatrix = glGetUniformLocation(shaderProgram, "viewMatrix");
	unsigned int loc_normalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
	unsigned int loc_modelMatrix = glGetUniformLocation(shaderProgram, "modelMatrix");
	
	glUniformMatrix4fv(loc_viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(loc_normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	unsigned int loc_cameraPosition = glGetUniformLocation(shaderProgram, "cameraPosition");
	//unsigned int loc_cameraDirection = glGetUniformLocation(shaderProgram, "cameraDirection");

	glUniform3f(loc_cameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	unsigned int loc_specularAmount = glGetUniformLocation(shaderProgram, "specularAmount");
	unsigned int loc_diffuseAmount = glGetUniformLocation(shaderProgram, "diffuseAmount");
	glUniform1f(loc_specularAmount, specularAmount);
	glUniform1f(loc_diffuseAmount, diffuseAmount);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size()*3);
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	
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
	if (keyboard[SDL_SCANCODE_W]) position += glm::vec3(0, 0, -1) * dt;
	if (keyboard[SDL_SCANCODE_S]) position += glm::vec3(0, 0, 1) * dt;
	if (keyboard[SDL_SCANCODE_D]) rotation += glm::vec3(1, 0, 0) * dt;
	if (keyboard[SDL_SCANCODE_A]) rotation += glm::vec3(-1, 0, 0) * dt;

	glm::vec3 cameraFront = glm::rotate(glm::mat4(1.0f), cameraRotation.x, glm::vec3(0, 1, 0)) * glm::vec4(0,0,-1,0);
	if (keyboard[SDL_SCANCODE_I]) cameraPosition += 3.0f * cameraFront * dt;
	if (keyboard[SDL_SCANCODE_K]) cameraPosition += -3.0f * cameraFront * dt;
	if (keyboard[SDL_SCANCODE_L]) cameraRotation += 2.0f * glm::vec3(-1, 0, 0) * dt;
	if (keyboard[SDL_SCANCODE_J]) cameraRotation += 2.0f * glm::vec3(1, 0, 0) * dt;

	if (keyboard[SDL_SCANCODE_PAGEUP]) {   specularAmount += dt; std::cout <<"Specular: "<< specularAmount << std::endl;}
	if (keyboard[SDL_SCANCODE_PAGEDOWN]) { specularAmount -= dt; std::cout <<"Specular: "<< specularAmount << std::endl;}

	if (keyboard[SDL_SCANCODE_HOME]) {   diffuseAmount += dt; std::cout <<   "Diffuse: " << diffuseAmount << std::endl; }
	if (keyboard[SDL_SCANCODE_END]) {    diffuseAmount -= dt; std::cout <<   "Diffuse: " << diffuseAmount << std::endl; }
	
}
