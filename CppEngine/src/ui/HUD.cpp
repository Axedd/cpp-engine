#include "ui/HUD.h"
#include <string>


void HUD::init(SDL_Renderer* r, TTF_Font* font) {
	scoreLabel.setFont(font);
	livesLabel.setFont(font);
	healthLabel.setFont(font);

	SDL_Color white{ 255,255,255,255 };
	scoreLabel.setColor(white);
	livesLabel.setColor(white);
	healthLabel.setColor(white);

	// force initial build
	m_Last.score = -1;
	m_Last.lives = -1;
	m_Last.health = -1;
}


// Updates the HUD data if no new data it no update occurs
void HUD::update(SDL_Renderer* r, const HUDData& data) {
	if (data.score != m_Last.score) {
		scoreLabel.setText(r, "Score: " + std::to_string(data.score));
		m_Last.score = data.score;
	}
	if (data.lives != m_Last.lives) {
		livesLabel.setText(r, "Lives: " + std::to_string(data.lives));
		m_Last.lives = data.lives;
	}
	if (data.health != m_Last.health) {
		healthLabel.setText(r, "HP: " + std::to_string(data.health));
		m_Last.health = data.health;
	}
}

void HUD::render(SDL_Renderer* r, int screenW, int screenH) {
	// top-left UI
	scoreLabel.render(r, 12, 12);
	livesLabel.render(r, 12, 40);
	healthLabel.render(r, 12, 68);

	// Rects goes here in the future
}