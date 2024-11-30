#ifndef TGAME_H
#define TGAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "vec.hpp"//vec3
#include "vec2.hpp"//vec2
#include <array>
#include <iostream>
#include <fstream>
#include "T4DTriangle.hpp"
#include "t4dutils.hpp"
#include "TCamera.hpp"
#include <chrono>
#include <eigen3/Eigen/Eigen>
#include <string>

namespace sc = std::chrono;
using namespace Eigen;

class GameEngine {
    private:
        vec3 position = {1, 1, 100};
        float dfov = 100;
        int Winwidth=0;
        int winHeight=0;
        char *memblock;
        const unsigned char *keys;
        vec3 cameraPos;
        float camrot = 0;
        bool initialized = false;
        float viewDistance = 10096;
    public:
        float frametime = 0;
        int frameCount = 0;
        bool logging = false;
        bool GameQuit = false;
        SDL_Renderer* renderer = nullptr;
        SDL_Window *window = nullptr;
        typedef std::chrono::high_resolution_clock Time;
        typedef std::chrono::milliseconds ms;
        typedef std::chrono::duration<float> fsec;
        std::chrono::system_clock::time_point t1 = Time::now();
        std::chrono::system_clock::time_point t0 = Time::now();
        TCamera used_cam;
        SDL_Texture* stoneIMG;
        void Init(int width, int height) {
            if (SDL_Init(SDL_INIT_VIDEO)<0) std::cerr << "Failed to initialize sdl\n";
            //SDL_Delay(1000);
            SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
            if (SDL_CreateWindowAndRenderer(width, height, SDL_RENDERER_ACCELERATED, &window, &renderer) != 0) std::cerr << "Failed to create sdl render\n";
            SDL_SetWindowTitle(window, "Tino 3d engine");
            keys = SDL_GetKeyboardState(NULL);
            stoneIMG = IMG_LoadTexture(renderer, "./res/stone.png");
            SDL_RenderPresent(renderer);
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            Winwidth = width;
            winHeight = height;
            initialized = true;
        }
        void DebugLog(std::string output) {
            if (logging) std::cout << output << "\n";
        }
        void halt(int durationMs) {
            SDL_Delay(durationMs);
        }

        void HandleEvents() {
            SDL_Event event;
            if (SDL_QuitRequested()) {
                GameQuit = true;
            }
            while (SDL_PollEvent(&event)) {  // poll until all events are handled!
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    GameQuit = true;
                }
            }

            SDL_PumpEvents();
        }

        bool isKeyDown(SDL_Scancode key) {
            if (keys[key]) {
                return true;
            } return false;
        }

        void clearBackground(short r, short g, short b, short a) {
            if (!initialized) {
                std::cerr << "Engine has not yet been initialized!! Call engine.init(width, height) to initialize it.\n";
                return;
            }
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderClear(renderer);
        }
        float getFrameTime() {
            fsec ws = t1 - t0;
            ms dt = std::chrono::duration_cast<ms>(ws);
            
            return (float)1+dt.count();
        }

        float render() {
            SDL_Delay(4);
            SDL_RenderPresent(renderer);
            if (isKeyDown(SDL_SCANCODE_ESCAPE)) close();
            t1 = Time::now();
            frameCount++;
            if (frameCount==100) {
                std::cout << frametime << " : "<< 1000/(frametime) << "\n\n\n";
                frameCount=0;
            }
            frametime = getFrameTime();
            
            t0 = Time::now();
            if (frametime<10) {
                //SDL_Delay(10-getFrameTime());
            }
            return frametime;
        }
        void close() {
            SDL_DestroyTexture(stoneIMG);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            GameQuit = true;
        }

        vec2 projected(vec3 pos) {
            // Clamping z to avoid division by zero or negative z values
            float z_min = 0.01f; // Near clipping plane
            float z_clamped = std::max(pos.z() + dfov, z_min);

            // Create the perspective projection matrix
            Matrix4f projection;
            projection.setZero();
            projection(0, 0) = dfov; // FOV scaling for x
            projection(1, 1) = dfov; // FOV scaling for y
            projection(2, 2) = 1.0f; // Depth scaling (unused here)
            projection(3, 2) = 1.0f; // Homogeneous coordinate
            projection(3, 3) = 0.0f; // Ensure w = z for division

            // Convert 3D point to 4D vector for matrix multiplication
            Vector4f pos4D(pos.x(), pos.y(), pos.z(), 1.0f);

            // Apply projection matrix
            Vector4f projected4D = projection * pos4D;

            // Perspective division to get normalized device coordinates (NDC)
            Vector3f projected3D(projected4D.x() / z_clamped,
                                projected4D.y() / z_clamped,
                                0.0f);

            // Translate to screen space
            vec2 screenOffset(Winwidth / 2.0f, winHeight / 2.0f);
            return vec2(projected3D.x(), projected3D.y()) + screenOffset;
        }
        
        vec3 rotateToCamera(vec3 relativePosition) {
            float sinCamRot = sin(used_cam.rotation);
            float cosCamRot = cos(used_cam.rotation);
            vec3 rotatedPosition(
                relativePosition.x() * cosCamRot - relativePosition.z() * sinCamRot,
                relativePosition.e[1], // Keep the y-component unchanged
                relativePosition.x() * sinCamRot + relativePosition.z() * cosCamRot
            );
            return rotatedPosition;
        }
        vec3 relativeToCamera(vec3 pos) {
            return pos - cameraPos;}
        vec3 getPos() {
            return position;
        }
        //Draw 3d stuff definitions
        void DrawLine3D(vec3 positionStart, vec3 PositionEnd, vec3 color=vec3(255), float alpha=1) {
            vec2 projectedStart = projected(positionStart);
            vec2 ProjectedEnd   = projected(PositionEnd);

            //Outside of the screen
            if (projectedStart.x()>Winwidth) if(ProjectedEnd.x()>Winwidth) { return; }
            if (projectedStart.x()<0) if(ProjectedEnd.x()<0) { return; }
            if (projectedStart.y()>winHeight) if (ProjectedEnd.y()>winHeight) return;
            if (projectedStart.y()<0)         if (ProjectedEnd.y()<0) return;

            SDL_SetRenderDrawColor(renderer, color.x(), color.y(), color.z(), 255*alpha);
            SDL_RenderDrawLineF(renderer, projectedStart.x(), projectedStart.y(), ProjectedEnd.x(), ProjectedEnd.y());
        }
        void DrawCrossCube3D(vec3 position, float size, float rotation, vec3 color) {//Actually useful
            // Check if the cube is within the renderable range
            vec3 cameraToPosition = position - cameraPos;
            float distance = cameraToPosition.length();
            rotation-=used_cam.rotation;
            if (distance < 5000 && distance > 5) {

                // Translate the position relative to the camera
                vec3 relativePosition = position - cameraPos;
                vec3 rotatedPosition = rotateToCamera(relativePosition);
                
                // Calculate the vertices for the bottom face of the cube
                vec3 bottomVerts[4] = {
                    vec3(rotatedPosition.x() + -size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * (-size)),
                    vec3(rotatedPosition.x() + -size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * (-size)),
                    vec3(rotatedPosition.x() +  size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * size),
                    vec3(rotatedPosition.x() +  size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * size),
                };

                // Draw bottom face edges
                for (int i = 0; i < 3; i++) {
                    DrawLine3D(bottomVerts[i], bottomVerts[i + 1], color);
                }
                DrawLine3D(bottomVerts[0], bottomVerts[3], color);

                // Calculate vertices for the top face (shifted upward by size)
                vec3 topVerts[4];
                for (int i = 0; i < 4; i++) {
                    topVerts[i] = bottomVerts[i];
                    topVerts[i].e[1] -= size; // Adjust the y-component
                }

                // Draw top face edges
                for (int i = 0; i < 3; i++) {
                    DrawLine3D(topVerts[i], topVerts[i + 1], color);
                }
                DrawLine3D(topVerts[0], topVerts[3], color);
                
                // Draw vertical edges connecting top and bottom faces
                for (int i = 0; i < 4; i++) {
                    DrawLine3D(topVerts[i], bottomVerts[i], color);
                }
            }
        }
        void DrawCubicPlane(vec3 position, float size, float rotation, vec3 color, float alpha=1) {//Actually useful
            // Check if the cube is within the renderable range
            vec3 cameraToPosition = position - cameraPos;
            float distance = cameraToPosition.length();
            rotation-=used_cam.rotation;
            if (distance > viewDistance || distance < 5) return;
            {
                // Translate the position relative to the camera
                vec3 relativePosition = position - cameraPos;
                vec3 rotatedPosition = rotateToCamera(relativePosition);
                
                // Calculate the vertices for the bottom face of the cube
                vec3 bottomVerts[4] = {
                    vec3(rotatedPosition.x() + -size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * (-size)),
                    vec3(rotatedPosition.x() + -size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * (-size)),
                    vec3(rotatedPosition.x() +  size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * size),
                    vec3(rotatedPosition.x() +  size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * size),
                };

                // Draw bottom face edges
                for (int i = 0; i < 3; i++) {
                    DrawLine3D(bottomVerts[i], bottomVerts[i + 1], color, alpha);
                }
                DrawLine3D(bottomVerts[0], bottomVerts[3], color);
            }
        }
        void DrawImage3DGround(vec3 position, float size, float rotation) {
            vec3 cameraToPosition = position-cameraPos;
            float distance = cameraToPosition.length();
            rotation-=used_cam.rotation;

            if (distance > viewDistance*.8 || distance < 5) return;
            {
                // Translate the position relative to the camera
                vec3 relativePosition = position - cameraPos;
                vec3 rotatedPosition = rotateToCamera(relativePosition);
                // Calculate the vertices for the bottom face of the cube
                vec2 bottomVertsProjected[4] = {
                    projected(vec3(rotatedPosition.x() + -size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * (-size))),
                    projected(vec3(rotatedPosition.x() + -size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * (-size))),
                    projected(vec3(rotatedPosition.x() +  size * sin(rotation), rotatedPosition.y(), rotatedPosition.z()+cos(rotation) * size)),
                    projected(vec3(rotatedPosition.x() +  size * cos(-rotation), rotatedPosition.y(), rotatedPosition.z()+sin(-rotation) * size)),
                };

                // Project the corners
                SDL_Vertex vertices[4];
                vertices[0].position.x = bottomVertsProjected[0].x(); 
                vertices[0].position.y = bottomVertsProjected[0].y(); // Top-left
                vertices[0].tex_coord.x = 0;   vertices[0].tex_coord.y = 0;
                vertices[0].color = (SDL_Color){255, 255, 255, 255};       // No tint

                vertices[1].position.x = bottomVertsProjected[1].x(); 
                vertices[1].position.y = bottomVertsProjected[1].y(); // Top-right
                vertices[1].tex_coord.x = 1;   vertices[1].tex_coord.y = 0;
                vertices[1].color = (SDL_Color){255, 255, 255, 255};       // No tint

                vertices[2].position.x = bottomVertsProjected[2].x(); 
                vertices[2].position.y = bottomVertsProjected[2].y(); // Bottom-right
                vertices[2].tex_coord.x = 1;   vertices[2].tex_coord.y = 1;
                vertices[2].color = (SDL_Color){255, 255, 255, 255};       // No tint

                vertices[3].position.x = bottomVertsProjected[3].x(); 
                vertices[3].position.y = bottomVertsProjected[3].y(); // Bottom-left
                vertices[3].tex_coord.x = 0;   vertices[3].tex_coord.y = 1;
                vertices[3].color = (SDL_Color){255, 255, 255, 255};       // No tint
                const static int indices[] = {0, 1, 2, 2, 3, 0};
            
                SDL_RenderGeometry(renderer, stoneIMG, vertices, 4, indices, 6);
            }
        }
        void DrawImage3DBillBoard(vec3 position, float size, float rotation);
        void DrawImage3DWall(vec3 position, float size, float rotation);
        void DrawImage2D(int x1, int y1, int x2, int y2) {
            if (!stoneIMG) {
                std::cerr << SDL_GetError() << "\n";
                close();
            }
            
            SDL_Vertex vertices[4];
            vertices[0].position.x = 100; vertices[0].position.y = 100; // Top-left
            vertices[0].tex_coord.x = 0;   vertices[0].tex_coord.y = 0;
            vertices[0].color = (SDL_Color){255, 255, 255, 255};       // No tint

            vertices[1].position.x = 300; vertices[1].position.y = 100; // Top-right
            vertices[1].tex_coord.x = 1;   vertices[1].tex_coord.y = 0;
            vertices[1].color = (SDL_Color){255, 255, 255, 255};       // No tint

            vertices[2].position.x = 300; vertices[2].position.y = 500; // Bottom-right
            vertices[2].tex_coord.x = 1;   vertices[2].tex_coord.y = 1;
            vertices[2].color = (SDL_Color){255, 255, 255, 255};       // No tint

            vertices[3].position.x = 100; vertices[3].position.y = 300; // Bottom-left
            vertices[3].tex_coord.x = 0;   vertices[3].tex_coord.y = 1;
            vertices[3].color = (SDL_Color){255, 255, 255, 255};       // No tint
            int indices[] = {0, 1, 2, 2, 3, 0};
            
            SDL_RenderGeometry(renderer, stoneIMG, vertices, 4, indices, 6);
            //SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
        void MouseCaptured(bool yesNo) {
            if (yesNo) SDL_SetRelativeMouseMode(SDL_TRUE);
            else SDL_SetRelativeMouseMode(SDL_FALSE);
        }
        void DrawTriangle(Triangle triangle, float scale) {
            if (true) {
                vec3 sincosvec(sin(camrot), 1, cos(camrot));
                vec3 mainpos = triangle.Mainpos-cameraPos;
                mainpos=triangle.Mainpos+mainpos*sincosvec;

                vec3 vert1 = triangle.vertices[0]*scale;
                vec3 vert2 = triangle.vertices[1]*scale;
                vec3 vert3 = triangle.vertices[2]*scale;

                vert1 = mainpos + vec3(vert1.x(), vert1.y(), vert1.z());
                vert2 = mainpos + vec3(vert2.x(), vert2.y(), vert2.z());
                vert3 = mainpos + vec3(vert3.x(), vert3.y(), vert3.z());

                DrawLine3D(vert1, vert2);
                DrawLine3D(vert2, vert3);
                DrawLine3D(vert3, vert1);
            }
        }
        void useCamera(TCamera cam) {
            used_cam = cam;
            camrot = cam.rotation;
            dfov = cam.getFov();
            cameraPos = cam.getPos();
        }
        void DrawLine2D3D(int startx, int starty, vec3 end) {
            //end-=cameraPos;
            SDL_RenderDrawLineF(renderer, startx, starty, projected(end).x(), projected(end).y());
        }
        void DrawGrid(int lines, int spacing) {
            for (int x = 0; x < lines; x++) {
                DrawLine3D(vec3(-(spacing*lines), 100, spacing*x)-cameraPos, vec3((spacing*lines), 100, spacing*x)-cameraPos);
            }
            for (int y = 0; y < lines; y++) {
                DrawLine3D(vec3(y*spacing, 100, (spacing*lines)+y*spacing), vec3(0, 100, (spacing*lines)+y*spacing));
            }
        }
};

class ModelLoader {
    private:
        vec3 * vertices;
        char * memblock;
    public:

    void LoadOBJ()
    {
        
        int maxlines = 507;
        //int maxlines = 3;
        int line = 0;
        int charpos = 68;
        std::ifstream objFile("Suzanne.obj", std::ios::in | std::ios::binary | std::ios::ate);
        if (objFile.is_open()) {
            std::clog << "file opened!\n";
            std::streampos size;
            size = objFile.tellg();
            std::clog << "file size is: " << size << "\n";
            memblock = new char[size];
            objFile.seekg(0, std::ios::beg);
            objFile.read(memblock, size);
            objFile.close();
        }
        else  {
            std::cerr << "Failed to open file!\n";
        }
        vertices = new vec3[maxlines];
        std::string tmp_chars = "";
        while (line < maxlines) {
            if (memblock[charpos] == '\n') {
                tmp_chars+=" ";
                vertices[line] = vec3(splitterT4D(tmp_chars, 0, ' '), splitterT4D(tmp_chars, 1, ' '), splitterT4D(tmp_chars, 2, ' '));
                std::clog << vertices[line].z() << "\n";
                line++;
                std::clog << tmp_chars << "\n";
                tmp_chars = "";
            }
            charpos++;
            tmp_chars += memblock[charpos];
        }
        //Loaded vertices
        std::clog << "Loaded " << line << " vertices!\n";
        
        
        //Load Faces
        while (true) {
            charpos++;
            char curr = memblock[charpos];
            if (memblock[charpos] == 'f') {
                break;
            }
        }//search for the faces
        tmp_chars = "";
        line = 0;
        while (line < 480) {

            if ( memblock[charpos] == '\n') {
                //std::clog << "\n\n" << tmp_chars << memblock[charpos] << "\n";

                line++;
                tmp_chars = "";
            }
            charpos++;
            tmp_chars += memblock[charpos];
        }

    }

    vec3 getVertPos(int vertnum) {
        vertices[vertnum].e[1] *= -1;
        return vertices[vertnum];
    }

    void cleanUp()
    {
        delete[] memblock;
        delete[] vertices;
    }
};

void GameEngine::DrawImage3DBillBoard(vec3 position, float size, float rotation) {
    vec3 cameraToPosition = position-cameraPos;
    float distance = cameraToPosition.length();
    rotation-=used_cam.rotation;
    if (distance > viewDistance*.8 || distance < 5) return;
    {
        // Translate the position relative to the camera
        vec3 relativePosition = position - cameraPos;
        vec3 rotatedPosition = rotateToCamera(relativePosition);
        vec2 projPos = projected(rotatedPosition);

        if (projPos.x()<0 || projPos.x()>Winwidth+size) {
            DebugLog("Image out of window not, rendering");
            return;
        }

        
        // Calculate the vertices for the bottom face of the cube        
        vec2 bottomVertsProjected[4] = {
                    projPos-vec2(-size, -size),//top left
                    projPos-vec2(size, -size),//top left
                    projPos-vec2(size, size),//top left
                    projPos-vec2(-size, size),//top left
        };


        // Project the corners
        SDL_Vertex vertices[4];
        vertices[0].position.x = bottomVertsProjected[0].x();
        vertices[0].position.y = bottomVertsProjected[0].y(); // Top-left
        vertices[0].tex_coord.x = 0;   vertices[0].tex_coord.y = 0;
        vertices[0].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[1].position.x = bottomVertsProjected[1].x();
        vertices[1].position.y = bottomVertsProjected[1].y(); // Top-right
        vertices[1].tex_coord.x = 1;   vertices[1].tex_coord.y = 0;
        vertices[1].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[2].position.x = bottomVertsProjected[2].x();
        vertices[2].position.y = bottomVertsProjected[2].y(); // Bottom-right
        vertices[2].tex_coord.x = 1;   vertices[2].tex_coord.y = 1;
        vertices[2].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[3].position.x = bottomVertsProjected[3].x();
        vertices[3].position.y = bottomVertsProjected[3].y(); // Bottom-left
        vertices[3].tex_coord.x = 0;   vertices[3].tex_coord.y = 1;
        vertices[3].color = (SDL_Color){255, 255, 255, 255};       // No tint
        const static int indices[] = {0, 1, 2, 2, 3, 0};
    
        SDL_RenderGeometry(renderer, stoneIMG, vertices, 4, indices, 6);
    }
}

void GameEngine::DrawImage3DWall(vec3 position, float size, float rotation) {
    vec3 cameraToPosition = position-cameraPos;
    float distance = cameraToPosition.length();
    rotation-=used_cam.rotation;
    if (distance > viewDistance*.8 || distance < 5) return;
    {
        // Translate the position relative to the camera
        vec3 relativePosition = position - cameraPos;
        vec3 rotatedPosition = rotateToCamera(relativePosition);
        if (projected(rotatedPosition).x()<0 || projected(rotatedPosition).x()>Winwidth+size) {
            DebugLog("Image out of window not, rendering");
            return;
        }
        vec2 bottomVertsProjected[4] = {
                    projected(vec3(rotatedPosition.x() -  size * sin(rotation),  rotatedPosition.y() ,  rotatedPosition.z()+cos(rotation) * (-size))),//top left
                    projected(vec3(rotatedPosition.x() -  size * cos(-rotation), rotatedPosition.y() , rotatedPosition.z()+sin(-rotation) * (-size))),
                    projected(vec3(rotatedPosition.x() -  size * cos(-rotation), rotatedPosition.y() +size, rotatedPosition.z()+sin(-rotation) * (-size))),
                    projected(vec3(rotatedPosition.x() -  size * sin(rotation),  rotatedPosition.y() +size,  rotatedPosition.z()+cos(rotation) * (-size))),//top left
            
        };

        // Project the corners
        SDL_Vertex vertices[4];
        vertices[0].position.x = bottomVertsProjected[0].x();
        vertices[0].position.y = bottomVertsProjected[0].y(); // Top-left
        vertices[0].tex_coord.x = 0;   vertices[0].tex_coord.y = 0;
        vertices[0].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[1].position.x = bottomVertsProjected[1].x();
        vertices[1].position.y = bottomVertsProjected[1].y(); // Top-right
        vertices[1].tex_coord.x = 1;   vertices[1].tex_coord.y = 0;
        vertices[1].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[2].position.x = bottomVertsProjected[2].x();
        vertices[2].position.y = bottomVertsProjected[2].y(); // Bottom-right
        vertices[2].tex_coord.x = 1;   vertices[2].tex_coord.y = 1;
        vertices[2].color = (SDL_Color){255, 255, 255, 255};       // No tint

        vertices[3].position.x = bottomVertsProjected[3].x();
        vertices[3].position.y = bottomVertsProjected[3].y(); // Bottom-left
        vertices[3].tex_coord.x = 0;   vertices[3].tex_coord.y = 1;
        vertices[3].color = (SDL_Color){255, 255, 255, 255};       // No tint
        const static int indices[] = {0, 1, 2, 2, 3, 0};
    
        SDL_RenderGeometry(renderer, stoneIMG, vertices, 4, indices, 6);
    }
}

bool isOddNumber(int n) {
    if ((float)n/2==(int)(n/2)) return false;
    return true;
}

class BallPhysicsBoundaries {
    private:
    public:
};


#endif