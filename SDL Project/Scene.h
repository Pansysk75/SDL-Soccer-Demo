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
	std::vector<Light_Point> pointLights;
    Player player;
    Goalpost goalpost1;
    Goalpost goalpost2;
    Field field;

    public:

    void Load() {
        skybox.Load();
        player.Load();
        field.Load();
        goalpost1.Load();
        goalpost1.SetPosition(glm::vec3(-40, 0, 0));
        goalpost2.Load();
        goalpost2.SetPosition(glm::vec3(40, 0, 0));
        goalpost2.SetRotation(glm::vec3(glm::radians(180.0f), 0, 0));

        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), 2.0f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), 0.5f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(1.0f, -0.5f, -1.0f), 0.2f));
        lights.push_back(Light(glm::vec3(0.7f, 0.7f, 1.0f), glm::vec3(0.4f, -0.7f, -0.2f), 0.2f));
        pointLights.push_back(Light_Point());

    }

    void Update(float dt){
        player.Update(dt);
        pointLights[0].position = player.ball.position;
    }

    void Render() {
        skybox.Render(player.GetCamera());
        player.Render(lights,pointLights);
        field.Render(player.GetCamera(), lights,pointLights);
        goalpost1.Render(player.GetCamera(), lights,pointLights);
        goalpost2.Render(player.GetCamera(), lights,pointLights);

    }

};