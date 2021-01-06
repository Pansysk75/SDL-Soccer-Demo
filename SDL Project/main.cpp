//Using SDL and standard IO
#include <Windows.h>
#include <SDL\SDL.h>
#include <GL\glew.h>
#include <assimp/Importer.hpp>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[])
{
 
    //Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
   
    //Create window
    SDL_Window* window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
     if (window == NULL) printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
     SDL_GL_CreateContext(window);
     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


     if (glewInit() != GLEW_OK) printf("Couldn't init glew");
        

       //Event handler
       SDL_Event e;
       //While application is running
       while (true)
       {
           //Handle events on queue
           while (SDL_PollEvent(&e) != 0)
           {
               if (e.type == SDL_QUIT) SDL_Quit();
           }

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

           glBegin(GL_TRIANGLES);
           glColor3f(0.8f, 0.3f, 0.3f);
           glVertex2f(0, 0);
           glVertex2f(-0.1f, -0.1f);
           glVertex2f(0, 0.1f);
           glEnd();


           SDL_GL_SwapWindow(window);
       }
        
    
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}