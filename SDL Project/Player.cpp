#include "Player.h"
#include <SDL\SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL\glew.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <filesystem>
#include <string>




void Player::Load(){
		model.Load("player");
		skybox.Load();
		position = glm::vec3(0,0,0);
		rotation = glm::vec3(0, 0, 0);

		cameraPosition = glm::vec3(0, 0, 0);
		cameraRotation = glm::vec3(0, 0, 0);
		diffuseAmount = 1.0f;
		specularAmount = 1.0f;
	}



void Player::Render() {

	
	//update camera
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
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

	unsigned int shaderProgram = this->model.shaderProgram;

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
	glBindTexture(GL_TEXTURE_2D, this->model.texture);

	glBindVertexArray(this->model.VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->model.EBO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size()*3);
	glDrawElements(GL_TRIANGLES, this->model.mesh.indices.size(), GL_UNSIGNED_INT, 0);
	
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
