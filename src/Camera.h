#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

class Camera {
public:
	Camera(int width, int height, int levelWidth, int levelHeight);

	void update(int targetX, int targetY, int targetW, int targetH, float deltaTime);

	SDL_Rect getRect() const { return m_view; }


	void shake(float magnitude, float duration);

	int getRenderX() const { return m_view.x + m_offsetX; }
	int getRenderY() const { return m_view.y + m_offsetY; }

	int x, y;

private:
	SDL_Rect m_view;
	int m_levelWidth, m_levelHeight;
	
	float m_shakeMagnitude = 0.0f;
	float m_shakeDuration = 0.0f;
	float m_shakeTimer = 0.0f;

	int m_offsetX = 0; 
	int m_offsetY = 0;
};



#endif