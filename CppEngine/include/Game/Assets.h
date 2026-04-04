#pragma once
#include <SDL.h>


struct SpriteSheet {
    unsigned int textureID = 0; // OpenGL handle
    int frameW = 0;
    int frameH = 0;
    int frames = 0;
    float fps = 0.0f;
    bool loop = true;
};

struct GameAssets {
    SpriteSheet coin;
};