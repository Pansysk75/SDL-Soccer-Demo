#pragma once
#include <glm/vec2.hpp>

class Mouse {
	inline static glm::vec2 position = glm::vec2(0);
	inline static glm::vec2 relativePosition = glm::vec2(0);

public:

	static void Update(glm::vec2 pos) { relativePosition = pos - position; position = pos; }
	static void Update(float x, float y) { Update( glm::vec2(x, y) ); }

	static void UpdateRel(glm::vec2 relPos) { relativePosition = relPos; }
	static void UpdateRel(float xrel, float yrel) { UpdateRel( glm::vec2(xrel, yrel) ); }

	static glm::vec2 GetPosition() { return position; }
	static glm::vec2 GetRelativePosition() { return relativePosition; }
};