#include "Player.h"
#include <SDL\SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Mouse.h"

#include <GL\glew.h>
#include <iostream>
#include <string>




void Player::Load(){
		model.Load("player");
		skybox.Load();
		position = glm::vec3(0,2,0);
		rotation = glm::vec3(0, 0, 0);
		collisionMesh.Import("Player_CollisionMesh");

	}



void Player::Render() {

	skybox.Render(camera);
	model.Render(camera);

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

	SDL_Event event;
	
	
	camera.Rotate(Mouse::GetRelativePosition().x * glm::vec3(-1, 0, 0) * dt );
	camera.Rotate(Mouse::GetRelativePosition().y * glm::vec3(0, -1, 0) * dt );
	
	if (keyboard[SDL_SCANCODE_I]) camera.Move(3.0f * glm::vec3(0,0,-1) * dt);
	if (keyboard[SDL_SCANCODE_K]) camera.Move(3.0f * glm::vec3(0, 0, 1) * dt);
	if (keyboard[SDL_SCANCODE_J]) camera.Move(3.0f * glm::vec3(-1, 0, 0) * dt);
	if (keyboard[SDL_SCANCODE_L]) camera.Move(3.0f * glm::vec3(1, 0, 1) * dt);
	

	if (keyboard[SDL_SCANCODE_PAGEUP]) {   model.specularAmount += dt; std::cout <<"Specular: "<< model.specularAmount << std::endl;}
	if (keyboard[SDL_SCANCODE_PAGEDOWN]) { model.specularAmount -= dt; std::cout <<"Specular: "<< model.specularAmount << std::endl;}

	if (keyboard[SDL_SCANCODE_HOME]) {   model.diffuseAmount += dt; std::cout <<   "Diffuse: " << model.diffuseAmount << std::endl; }
	if (keyboard[SDL_SCANCODE_END]) {    model.diffuseAmount -= dt; std::cout <<   "Diffuse: " << model.diffuseAmount << std::endl; }

	camera.Update();
	model.position = position;
	model.rotation = rotation;
}
