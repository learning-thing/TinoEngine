#ifndef TCAM_H
#define TCAM_H

#include "vec.hpp"

class TCamera
{
private:
public:
    float fov = 50;
    float rotation = 0; 
    vec3 position = vec3(0);
    vec3 front;

    bool isBehindCamera(vec3 target) {
        front = position+vec3(sin(rotation), 0, cos(rotation))*.01;
        if ((position-target).length()<(front-target).length()) {//distance from the camera to target is greater that from the front to the target
            return false;
        }
        return true;
    }

    void setFOV(float targ)
    {
        fov = targ;
    }

    float getFov()
    {
        return fov;
    }
    vec3 getPos()
    {
        return position;
    }
};


#endif