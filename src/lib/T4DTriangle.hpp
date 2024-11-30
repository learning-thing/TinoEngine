#ifndef T4DTRINAGLE_H
#define T4DTRINAGLE_H
#include <math.h>
#include "vec.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

class Triangle {
    private:
    bool ffacing = false;

    public:
    float rotation = 0;
    vec3 Mainpos = vec3(0);
    vec3 Rawvertices[3] = {
        vec3(0),
        vec3(0),
        vec3(0),
    };

    vec3 vertices[3] = {
        vec3(0),
        vec3(0),
        vec3(0),
    };
    Triangle(vec3 pos, vec3 vert1, vec3 vert2, vec3 vert3, bool facingCam) {
        Mainpos = pos;
        Rawvertices[0] = vert1;
        Rawvertices[1] = vert2;
        Rawvertices[2] = vert3;
        
        ffacing = facingCam;
    }
    ~Triangle() {
    }

    void move(vec3 newPosi) {//update position and rotation
        Mainpos = newPosi;

        for (int i = 0; i < 3; i++) {
            vertices[i] = Rawvertices[i];
            vertices[i] *= vec3(sin(rotation), 1, cos(rotation));// * sin(rotation);
            std::clog << "Rotated: " << vertices[i].e[0] << " Raw: " << Rawvertices[i].e[0] << "\n";
            //vertices[i].e[2] = Rawvertices[0].e[2] * cos(rotation);
        }
    }

};

class Mesh {
    private:
        std::vector<Triangle> triangles;
    public:
        
};

#endif