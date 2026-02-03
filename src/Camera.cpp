#include "Camera.h"
#include <SDL.h>
#include <stdlib.h>

Camera::Camera(int width, int height, int levelWidth, int levelHeight)
    : m_levelWidth(levelWidth), m_levelHeight(levelHeight), m_w(width), m_h(height)
{
    m_x = 0;
    m_y = 0;
    x = 0;
    y = 0;
}

SDL_Rect Camera::getRect() const {
    return { (int)m_x, (int)m_y, m_w, m_h };
}

// Camera shake effect (explosive)
void Camera::shake(float magnitude, float duration) {
    m_shakeMagnitude = magnitude;
    m_shakeDuration = duration;
    m_shakeTimer = duration;
}

void Camera::update(int targetX, int targetY, int targetW, int targetH, float deltaTime) {
    // 1. Center the camera on the target
    m_x = (targetX + targetW / 2.0f) - (m_w / 2.0f); 
    m_y = (targetY + targetH / 2.0f) - (m_h / 2.0f);

    // 2. Clamp to level boundaries
    if (m_x < 0) m_x = 0;
    if (m_y < 0) m_y = 0;
    if (m_x > m_levelWidth - m_w) m_x = (float)(m_levelWidth - m_w);
    if (m_y > m_levelHeight - m_h) m_y = (float)(m_levelHeight - m_h);
  
    if (m_shakeTimer > 0) {
        m_shakeTimer -= deltaTime;
        float progress = m_shakeTimer / m_shakeDuration;
        float currentMag = m_shakeMagnitude * progress;

        if (currentMag > 1.0f) {
            int magInt = static_cast<int>(currentMag);
            m_offsetX = (rand() % magInt) - (magInt / 2);
            m_offsetY = (rand() % magInt) - (magInt / 2);
        }
    } else {
        m_offsetX = 0; m_offsetY = 0;
    }

    x = m_x;
    y = m_y;
}

