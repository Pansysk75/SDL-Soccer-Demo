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
		ball.Load();
		skybox.Load();
		position = glm::vec3(0,2,0);
		rotation = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		collisionMesh.Import("Player_CollisionMesh");

		shootingPower = 20.0f;
		shootingLift = 0.6f;
		shootPowerBuildup = 0.0f;
	}



void Player::Render() {

	skybox.Render(camera);
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


	if (keyboard[SDL_SCANCODE_HOME]) {   shootingPower += dt * (shootingPower + 1) ; std::cout <<	"Power: " << shootingPower << std::endl; }
	if (keyboard[SDL_SCANCODE_PAGEUP]) { shootingPower -= dt * (shootingPower + 1) ; std::cout <<	"Power: "<<  shootingPower << std::endl;}

	if (keyboard[SDL_SCANCODE_PAGEDOWN]) {shootingLift += dt * (shootingLift + 1); std::cout <<  "Lift: " << shootingLift << std::endl;}
	if (keyboard[SDL_SCANCODE_END]) {	  shootingLift -= dt * (shootingLift + 1); std::cout <<  "Lift: " << shootingLift << std::endl; }

	//BALL
	ball.velocity += glm::vec3(0, -10, 0) * dt;
	if (ball.position.y - ball.radius < 0.0f) {
		ball.position.y = ball.radius + 0.0f;
		if (ball.velocity.y < 0) {
			ball.velocity.y = -ball.velocity.y ;
			ball.velocity = ball.velocity * 0.6f;
		}
	}
	if (keyboard[SDL_SCANCODE_RETURN]) {
		ball.velocity = glm::vec3(0.0f);
		ball.position = glm::vec3(0, 10, 0);
	}
	float playerRadius = 1.0f;
	if (glm::abs(position.x - ball.position.x) < ball.radius + playerRadius && glm::abs(position.z - ball.position.z) < ball.radius + playerRadius) {
		ball.velocity += (ball.position - position)*dt;
		//std::cout << "COLL" << std::endl;
		if (Mouse::IsLeftButtonDown()) {
			shootPowerBuildup += dt;
			if (shootPowerBuildup > 2.0f)shootPowerBuildup = 2.0f;
			std::cout << shootPowerBuildup << std::endl;
		} else if(shootPowerBuildup > 0){
			glm::vec3 cameraDirection = camera.GetLookDirection();
			cameraDirection.z = -cameraDirection.z;
			cameraDirection.y = 0.0f;
			glm::vec3 direction = glm::normalize(cameraDirection);
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
