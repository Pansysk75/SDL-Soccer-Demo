#pragma once
#include <glm/vec3.hpp>

struct Light {
	glm::vec3 color;
	glm::vec3 direction;
	float intensity;

	Light() 
		:color(1.0f), 
		direction(glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f))),
		intensity(1.0f) 
	{}

	Light(glm::vec3 color, glm::vec3 direction, float intensity) 
		:color(color),
		direction(glm::normalize(direction)),
		intensity(intensity)
	{}
};