//Using SDL and standard IO

//#include <Windows.h>
#include <SDL\SDL.h>
#include <GL\glew.h>
#include <assimp/Importer.hpp>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "Mouse.h"
#include "Scene.h"
#include "ResourceManager.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;


int main(int argc, char* args[])
{
 
    //Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
   
    //Create window
    SDL_Window* window = SDL_CreateWindow("SDL Geim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
     if (window == NULL) printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
     SDL_GL_CreateContext(window);
     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
     SDL_SetRelativeMouseMode(SDL_TRUE);

     if (glewInit() != GLEW_OK) printf("Couldn't init glew");

     //INITIALIZE GLOBAL STUFF
     ResourceManager::Initialize();


     Scene scene;
     scene.Load();

       //Event handler
       SDL_Event event;
       
       auto previousTime = std::chrono::steady_clock::now();
       glEnable(GL_DEPTH_TEST);



       //While application is running
       while (true)
       {
           //Handle events on queue
           
           while (SDL_PollEvent(&event) != 0)
           {
               if (event.type == SDL_QUIT) SDL_Quit();

           }

           int mouseX, mouseY;
           Uint32 mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
           Mouse::UpdateRel(mouseX, mouseY);
           Mouse::SetButtonStates(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT), mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));
           
 
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

           //time in seconds
           float dt = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - previousTime).count() * 0.000000001f;
           previousTime = std::chrono::steady_clock::now();

           scene.Update(dt);
           scene.Render();


           SDL_GL_SwapWindow(window);
       }


        
    
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}