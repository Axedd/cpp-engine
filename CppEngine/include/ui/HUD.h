#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include "TextLabel.h"


struct HUDData {
	int score = 0;
	int lives = 0;
	int health = 0;
};

class HUD {
public:
	void init(SDL_Renderer* r, TTF_Font* font);
	void update(SDL_Renderer* r, const HUDData& data);
	void render(SDL_Renderer* r, int screenW, int screenH);

private:
	HUDData m_Last{};
	TextLabel scoreLabel;
	TextLabel livesLabel;
	TextLabel healthLabel;
};