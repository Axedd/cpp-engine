#pragma once
#include <SDL.h>


struct SpriteSheet {
	SDL_Texture* tex = nullptr;
	int frameW = 0;
	int frameH = 0;
	int frames = 0;
	float fps = 0.0f;
	bool loop = true;
};


struct GameAssets {
	SpriteSheet coin;
};