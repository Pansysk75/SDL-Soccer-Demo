#pragma once
#include <glm/vec3.hpp>

struct Light_Point {
	glm::vec3 position;
	glm::vec3 color;
	float falloffLinear;
	float falloffQuadratic;
	float intensity;

	Light_Point()
		:position(0.0f),
		color(1.0f),
		falloffLinear(1.0f),
		falloffQuadratic(0.4f),
		intensity(1.0f)
	{}
};

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