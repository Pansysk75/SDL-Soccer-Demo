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
		velocity = glm::vec3(0, 0, 0);
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
	
	
	rotation += Mouse::GetRelativePosition().x * glm::vec3(-1, 0, 0) * dt ;
	rotation += Mouse::GetRelativePosition().y * glm::vec3(0, -1, 0) * dt ;

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(0, 1, 0));
	glm::vec3 forwardVec = rotationMatrix * glm::vec4(0,0,-1,0);
	glm::vec3 rightVec = glm::cross(forwardVec, glm::vec3(0, 1, 0));

	const float runSpeed = 3.0f;
	const float sprintSpeed = 6.0f;
	float moveSpeed;
	float strafeSpeed;
	if (keyboard[SDL_SCANCODE_LSHIFT]) { moveSpeed = sprintSpeed; strafeSpeed = runSpeed * 0.3f; }
	else { moveSpeed = runSpeed; strafeSpeed = runSpeed; }
	
	if (keyboard[SDL_SCANCODE_W]) position += moveSpeed * forwardVec * dt;
	if (keyboard[SDL_SCANCODE_S]) position += -moveSpeed * forwardVec * dt;
	if (keyboard[SDL_SCANCODE_D]) position +=  strafeSpeed * rightVec * dt;
	if (keyboard[SDL_SCANCODE_A]) position += -strafeSpeed * rightVec * dt;
	
	velocity += glm::vec3(0, -10, 0) * dt;
	if (position.y < 0.0f) { 
		position.y = 0.0f; 
		if (velocity.y < 0) {
			canJump = true;
			velocity.y = 0.0f;
		}
	}
	if (keyboard[SDL_SCANCODE_SPACE] && canJump) { 
		velocity += glm::vec3(0, 3, 0);
		canJump = false;
	}
	position += velocity * dt;


	if (keyboard[SDL_SCANCODE_PAGEUP]) {   model.specularAmount += dt; std::cout <<"Specular: "<< model.specularAmount << std::endl;}
	if (keyboard[SDL_SCANCODE_PAGEDOWN]) { model.specularAmount -= dt; std::cout <<"Specular: "<< model.specularAmount << std::endl;}

	if (keyboard[SDL_SCANCODE_HOME]) {   model.diffuseAmount += dt; std::cout <<   "Diffuse: " << model.diffuseAmount << std::endl; }
	if (keyboard[SDL_SCANCODE_END]) {    model.diffuseAmount -= dt; std::cout <<   "Diffuse: " << model.diffuseAmount << std::endl; }

	camera.position = position + glm::vec3(0, 1.6f, 0);
	camera.SetRotation( rotation);
	camera.Update();
	model.position = position;
	model.rotation = rotation;
}
