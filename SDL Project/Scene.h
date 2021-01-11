#pragma once
#include "Skybox.h"
#include "Light.h"
#include <vector>

class Scene {

	Skybox skybox;
	std::vector<Light> lights;

};