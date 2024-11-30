#pragma once
#include "vec.hpp"
#include <SDL2/SDL.h>
#include "Engine.hpp"

class MovementPositionControlelr {
    private:
        vec3 position = vec3(0,0,0);
        float deceleration=.7;
        float frameTime=0;
    public:
        float acceleration = 0;
        float Sacceleration = 0;
        float rotation=0;
        vec3 speed = vec3(0);
        vec3 AccelVec = vec3(0);
        vec3 SAccelVec = vec3(0);
        SDL_Scancode forwardKey=SDL_SCANCODE_W; 
        SDL_Scancode backwardkey=SDL_SCANCODE_S; 
        SDL_Scancode rightstrafekey=SDL_SCANCODE_D; 
        SDL_Scancode leftstrafekey=SDL_SCANCODE_A; 
        

        MovementPositionControlelr(vec3 startpos, vec3 initialspeed, float speedFallOff=.98) : position(startpos), speed(initialspeed), deceleration(speedFallOff) {

        }
        void update(float frametime, GameEngine* engine) {
            //frametime *= 60;
            if (engine->isKeyDown(forwardKey)) forward();
            if (engine->isKeyDown(backwardkey)) backward();
            if (engine->isKeyDown(leftstrafekey)) LeftStrafe();
            if (engine->isKeyDown(rightstrafekey)) RightStrafe();


            AccelVec = vec3(sin(rotation)*acceleration, 0, cos(rotation)*acceleration);
            SAccelVec = vec3(cos(-rotation)*Sacceleration, 0, sin(-rotation)*Sacceleration);

            frameTime = frametime;
            speed = ((speed+AccelVec+SAccelVec) * deceleration);
            
            acceleration=0;
            Sacceleration=0;
            position+=speed*frametime;
            if (position.y()>0) position.e[1]=0;
            //std::clog << position.x() << " -- " << position.z() << "\n";
            //std::clog << speed.length() << "\n" << "--" << speed.x() << " : " << speed.y() << " - " << AccelVec.x() << " : " << AccelVec.z() << "\n";
            
        }
        void RightStrafe() {Sacceleration = .5;}
        void LeftStrafe() {Sacceleration = -.5;}
        void forward() {acceleration = .5;}
        void backward() {acceleration = -.5;}

        vec3 getPos() {
            return position;
        }
};


class NPCMovement {
    private:
        vec3 position = vec3(0);
        vec3 speed = vec3(0);
        void Update() {
            position+=speed;
        }
    public:
        void FollowPlayer(vec3 PlayerPos) {
            speed += (PlayerPos-position)/100;
            Update();
        }
        vec3 getPos() {
            return position;
        }
};