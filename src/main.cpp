#include <iostream>
#include "Engine.hpp"
#include "SDL2/SDL.h"
#include "TCamera.hpp"
#include "TMovement.hpp"

int main() {
    std::cout << "Starting..." << "\n";
    GameEngine engine;
    TCamera cam;
    TCamera gunModelCam;
    gunModelCam.position = vec3(0);
    gunModelCam.fov = 300;
    gunModelCam.position = vec3(-6, 10, 20);
    MovementPositionControlelr movement(vec3(0), vec3(0));
    NPCMovement npc;
    engine.Init(1920, 1080);
    engine.MouseCaptured(true);
    float rotation = 3.9;
    float rotationSpeed = 0.001;
    float walkspeed = .1;
    float mrotSpeed;
    int mouseX;
    int mouseY;
    vec3 cubePos = vec3(500, 100, 400);

    while (!engine.GameQuit) {
        engine.useCamera(cam);
        engine.clearBackground(0, 0, 0, 1);
        int ss = 1000;
        engine.DrawLine2D3D(10, 10, vec3(0));
        for (int x = 0; x < 100; x++) for (int y = 0; y < 100; y++) {
            vec3 nPos = cubePos+vec3(ss*x, 100, ss*y);
            if (isOddNumber(x) && isOddNumber(y)) engine.DrawCubicPlane(nPos, ss/2, rotation, vec3(10, 100, 200), (nPos-cam.position).length());
            else engine.DrawCubicPlane(nPos, ss/2, rotation, vec3(100, 200, 10), (nPos-cam.position).length());
            //engine.DrawImage3DGround(nPos, ss/3, rotation);
            //engine.DrawImage2D(10, 10, 200, 200);
            //engine.DrawCrossCube3D(cubePos+vec3(500*x, 300, 500*y), 250, rotation, vec3(255, 255, 255));
            //engine.DrawAnyBox3D(cubePos+vec3(500*x, 300, 500*y), vec3(200, 20, 90), rotation, vec3(255, 255, 255));
        }
        engine.DrawImage3DBillBoard(cubePos-vec3(0, ss, 0), 10, rotation);
        //engine.DrawImage3DWall(cubePos-vec3(200, ss, 0), ss, rotation);

        engine.useCamera(gunModelCam);//Draw FPS stuff
        engine.DrawCrossCube3D(vec3(561, 261, 0), 100, 0, vec3(255));

        double frametime = engine.render()/2;
        engine.HandleEvents();
        //movement.rotation+=mrotSpeed;

        SDL_GetMouseState(&mouseX, &mouseY);
        movement.rotation = (float)mouseX/100;
        //vec3 Sidewardvec = vec3(cos(-cam.rotation), 0, sin(-cam.rotation));
        {
            cam.position = movement.getPos()-vec3(0, 30, 0);
            movement.update(engine.getFrameTime(), &engine);
            cam.rotation=movement.rotation;
            mrotSpeed*=.98;
            
            cam.fov = 500;//-movement.speed.length()*10;
            //gunModelCam.fov = 200-movement.speed.length()*2;
            if (engine.isKeyDown(SDL_SCANCODE_SPACE)) {rotation+=rotationSpeed*frametime; std::clog << rotation << "\n";}
 
            if (engine.isKeyDown(SDL_SCANCODE_PAGEUP)){
                cam.fov -= 1;
            }
            if (engine.isKeyDown(SDL_SCANCODE_PAGEDOWN)){
                cam.fov += 1;
            }

            if (engine.isKeyDown(SDL_SCANCODE_RIGHT)) {
                movement.rotation+=walkspeed*frametime*.1;
                //mrotSpeed+=.003;
            }
            if (engine.isKeyDown(SDL_SCANCODE_SPACE)) movement.speed.e[1] = -5;
            else { if (movement.getPos().y()<0) movement.speed.e[1] = frametime*2;}
            

            if (engine.isKeyDown(SDL_SCANCODE_LEFT)) {
                movement.rotation-=walkspeed*frametime*.1;
                //mrotSpeed-=.003;
            }

            if (engine.isKeyDown(SDL_SCANCODE_ESCAPE)) engine.close();
        }

    }
    
    return 0;
}


void controls() {
    
}