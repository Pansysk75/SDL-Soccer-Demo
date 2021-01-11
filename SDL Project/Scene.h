#pragma once
#include "Skybox.h"
#include "Light.h"
#include <vector>
#include "Field.h"
#include "Goalpost.h"
#include "Player.h"

class Scene {

	Skybox skybox;
	std::vector<Light> lights;
    Player player;
    Goalpost goalpost1;
    Goalpost goalpost2;
    Field field;

    public:

    void Load() {
        player.Load();
        field.Load();
        goalpost1.Load();
        goalpost1.SetPosition(glm::vec3(-40, 0, 0));
        goalpost2.Load();
        goalpost2.SetPosition(glm::vec3(40, 0, 0));
        goalpost2.SetRotation(glm::vec3(glm::radians(180.0f), 0, 0));

        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), 1.0f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), 0.5f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(1.0f, -0.5f, -1.0f), 0.5f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(0.4f, -0.7f, -0.2f), 0.5f));
    }

    void Update(float dt){
        player.Update(dt);
       

    }

    void Render() {
        player.Render(lights);
        field.Render(player.GetCamera(), lights);
        goalpost1.Render(player.GetCamera(), lights);
        goalpost2.Render(player.GetCamera(), lights);

    }
};