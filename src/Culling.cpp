#include <iostream>
#include "Engine.hpp"
#include "SDL2/SDL.h"
#include "TCamera.hpp"

int main()
{
    std::cout << "Starting..." << "\n";
    GameEngine engine;
    TCamera cam;

    Triangle  baseTriangle(vec3(0, -200, -100), vec3(0), vec3(100, 50, 0), vec3(0, 100, 0), true);
    Triangle baseTriangleB(vec3(0, -200, -100), vec3(0), vec3(-100, 50, 0), vec3(0, 100, 0), false);
    

    engine.Init(1080, 720);
    float r = 0;
    while (!engine.GameQuit)
    {
        engine.clearBackground(0, 0, 0, 1);
        //std::clog << "a\n";
        baseTriangle.move(vec3(0));//Updates the position
        baseTriangleB.move(vec3(0));//Updates the position
        engine.DrawTriangle(baseTriangle, 1);
        engine.DrawTriangle(baseTriangleB, 1);
        engine.useCamera(cam);

        {
            if (engine.isKeyDown(SDL_SCANCODE_W)) {
                cam.position+=vec3(sin(cam.rotation), 0, cos(cam.rotation));
            }
            if (engine.isKeyDown(SDL_SCANCODE_S)) {
                cam.position -= vec3(sin(cam.rotation), 0, cos(cam.rotation));
            }

            if (engine.isKeyDown(SDL_SCANCODE_A)) {
                cam.rotation+=.1;
            }

            if (engine.isKeyDown(SDL_SCANCODE_PAGEUP)) {
                cam.fov -= 1;
            }
            if (engine.isKeyDown(SDL_SCANCODE_PAGEDOWN)) {
                std::clog << cam.fov << "\n";
                cam.fov += 1;
            }
        }
        

        engine.render();
        engine.HandleEvents();
    }
    return 0;
}