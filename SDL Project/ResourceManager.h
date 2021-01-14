#pragma once
#include "Shader.h"

class ResourceManager {
	inline static Shader* shader;

public:
	static void Initialize() {
		shader = new Shader();
	}
	//void GetTexture();
	static Shader* GetShader() { return shader; }

};