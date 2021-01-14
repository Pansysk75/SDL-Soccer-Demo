#include "Player.h"
#include <SDL\SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Mouse.h"

#include <GL\glew.h>
#include <iostream>
#include <string>
#include "ResourceManager.h"




void Player::Load(){
		model.SetShader(ResourceManager::GetShader());
		model.Load("player");
		ball.Load();
		position = glm::vec3(0,2,0);
		rotation = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		collisionMesh.Import("Player_CollisionMesh");

		runSpeed = 30.0f;
		friction = 10.0f;

		hasBall = false;

		shootingPower = 20.0f;
		shootingLift = 0.6f;
		shootPowerBuildup = 0.0f;
	}



void Player::Render() {

	ball.Render(camera);
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
	camera.Rotate(Mouse::GetRelativePosition().x * glm::vec3(-1, 0, 0) * dt);
	camera.Rotate ( Mouse::GetRelativePosition().y * glm::vec3(0, -1, 0) * dt );

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(0, 1, 0));

	
	sprintSpeed = 2 * runSpeed;
	float moveSpeed;
	float strafeSpeed;

	if (keyboard[SDL_SCANCODE_LSHIFT]) { moveSpeed = sprintSpeed; strafeSpeed = runSpeed * 0.3f; }
	else { moveSpeed = runSpeed; strafeSpeed = runSpeed; }


	glm::vec4 accelerationDir = glm::vec4(0);
	
	if (keyboard[SDL_SCANCODE_W]) accelerationDir += glm::vec4(0, 0, -1, 0);
	if (keyboard[SDL_SCANCODE_S]) accelerationDir += glm::vec4(0, 0, 1, 0);
	if (keyboard[SDL_SCANCODE_D]) accelerationDir += glm::vec4(1, 0, 0, 0);
	if (keyboard[SDL_SCANCODE_A]) accelerationDir += glm::vec4(-1, 0, 0, 0);
	if (accelerationDir != glm::vec4(0)) { 
		accelerationDir = rotationMatrix * glm::normalize(accelerationDir);
		velocity += glm::vec3(accelerationDir) * moveSpeed * dt;
	}
		velocity.x += -velocity.x * dt * friction;
		velocity.z += -velocity.z * dt * friction;

	velocity += glm::vec3(0, -10, 0) * dt;
	if (position.y < 0.0f) { 
		position.y = 0.0f; 
		if (velocity.y < 0) {
			canJump = true;
			velocity.y = 0.0f;
			//velocity *= 0.95f;
		}
	}
	if (keyboard[SDL_SCANCODE_SPACE] && canJump) { 
		velocity += glm::vec3(0, 3, 0);
		canJump = false;
	}
	position += velocity * dt;

	/*
	if (keyboard[SDL_SCANCODE_HOME]) {   shootingPower += dt * (shootingPower + 1) ; std::cout <<	"Power: " << shootingPower << std::endl; }
	if (keyboard[SDL_SCANCODE_PAGEUP]) { shootingPower -= dt * (shootingPower + 1) ; std::cout <<	"Power: "<<  shootingPower << std::endl;}

	if (keyboard[SDL_SCANCODE_PAGEDOWN]) {shootingLift += dt * (shootingLift + 1); std::cout <<  "Lift: " << shootingLift << std::endl;}
	if (keyboard[SDL_SCANCODE_END]) {	  shootingLift -= dt * (shootingLift + 1); std::cout <<  "Lift: " << shootingLift << std::endl; }
	*/

	if (keyboard[SDL_SCANCODE_PAGEUP]) {  runSpeed -= dt * (runSpeed  + 1); std::cout << "Speed: "   << runSpeed << std::endl; }
	if (keyboard[SDL_SCANCODE_PAGEDOWN]) {runSpeed += dt * (runSpeed + 1);  std::cout << "Speed: "  <<  runSpeed  << std::endl; }

	if (keyboard[SDL_SCANCODE_HOME]) { friction += dt * (friction + 1); std::cout << "Friction: " << friction << std::endl; }
	if (keyboard[SDL_SCANCODE_END]) {  friction -= dt * (friction + 1);  std::cout <<"Friction: " << friction  << std::endl; }

	//BALL
	ball.velocity += glm::vec3(0, -10, 0) * dt;
	if (ball.position.y - ball.radius < 0.0f) {
		ball.position.y = ball.radius + 0.0f;
		if (ball.velocity.y < 0) {
			ball.velocity.y = -ball.velocity.y * 0.8f ;
			ball.velocity = ball.velocity * 0.98f;
		}
	}
	if (keyboard[SDL_SCANCODE_RETURN]) {
		ball.velocity = glm::vec3(0.0f);
		ball.position = glm::vec3(0.0f, 5.0f, 0.0f);
	}
	float playerRadius = 1.0f;
	float shootRadius = 1.0f;

	
	//for pushing ball
	if (keyboard[SDL_SCANCODE_LCTRL]) {
		if (glm::abs(position.x - ball.position.x) < ball.radius + playerRadius && glm::abs(position.z - ball.position.z) < ball.radius + playerRadius) {
			hasBall = true;
		}
		if (hasBall) {
			glm::vec3 offset = camera.GetLookDirection();
			float dx = (position.x + offset.x - ball.position.x);
			float dz = (position.z - offset.z - ball.position.z);
			ball.velocity.x += 600 *dx * dt ;
			ball.velocity.z += 600 *dz * dt ;
			ball.velocity.x *= 0.95f;
			ball.velocity.z *= 0.95f;
		}
	}
	else { hasBall = false; }

	//for shooting
	if (glm::abs(position.x - ball.position.x) < ball.radius + shootRadius && glm::abs(position.z - ball.position.z) < ball.radius + shootRadius) {
		
		//std::cout << "COLL" << std::endl;
		if (Mouse::IsLeftButtonDown()) {
			shootPowerBuildup += dt;
			if (shootPowerBuildup > 2.0f)shootPowerBuildup = 2.0f;
			std::cout << shootPowerBuildup << std::endl;
		} else if(shootPowerBuildup > 0){

			glm::vec3 cameraDirection = camera.GetLookDirection();
			cameraDirection.z = -cameraDirection.z;
			cameraDirection.y = 0.0f;
			cameraDirection = glm::normalize(cameraDirection);

			glm::vec3 movementDirection = velocity;
			movementDirection.y = 0.0;
			//if (movementDirection != glm::vec3(0))movementDirection = glm::normalize(movementDirection);

			//glm::vec3 direction = 3.0f * movementDirection;
			glm::vec3 direction = cameraDirection + 0.3f*movementDirection;
			direction.y = shootingLift;
			direction = glm::normalize(direction);
			
			if (shootPowerBuildup < 0.1f) shootPowerBuildup = 0.1f;
			ball.velocity += shootingPower * shootPowerBuildup * direction;
			shootPowerBuildup = 0.0f;
		}
	}
	else {
		shootPowerBuildup = 0.0f;
	}

	camera.position = position + glm::vec3(0, 1.8f, 0);
	
	camera.Update();
	ball.Update(dt);
	model.position = position;
	model.rotation = rotation;
}
