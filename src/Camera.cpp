#include "Camera.h"
#include <stdlib.h>

Camera::Camera(int width, int height, int levelWidth, int levelHeight)
    : m_levelWidth(levelWidth), m_levelHeight(levelHeight)
{
    m_view = { 0, 0, width, height };
    x = 0;
    y = 0;
}

// Camera shake effect (explosive)
void Camera::shake(float magnitude, float duration) {
    m_shakeMagnitude = magnitude;
    m_shakeDuration = duration;
    m_shakeTimer = duration;
}

void Camera::update(int targetX, int targetY, int targetW, int targetH, float deltaTime) {
    // 1. Center the camera rect on the target
    m_view.x = static_cast<int>(targetX + targetW / 2.0f) - (m_view.w / 2.0f); 
    m_view.y = static_cast<int>(targetY + targetH / 2.0f) - (m_view.h / 2.0f);

    // 2. Clamp to level boundaries so we don't see past the map
    if (m_view.x < 0) m_view.x = 0;
    if (m_view.y < 0) m_view.y = 0;
    if (m_view.x > m_levelWidth - m_view.w) m_view.x = m_levelWidth - m_view.w;
    if (m_view.y > m_levelHeight - m_view.h) m_view.y = m_levelHeight - m_view.h;
  
    if (m_shakeTimer > 0) {
        m_shakeTimer -= deltaTime;

        float progress = m_shakeTimer / m_shakeDuration;
        float currentMag = m_shakeMagnitude * progress;

        // Generate random offsets
        if (currentMag > 1.0f) {
            int magInt = static_cast<int>(currentMag);
            m_offsetX = (rand() % magInt) - (magInt / 2);
            m_offsetY = (rand() % magInt) - (magInt / 2);
        }
    }
    else {
        m_offsetX = 0;
        m_offsetY = 0;
        m_shakeMagnitude = 0;
    }

        
    // 3. Update public variables for the renderer
    x = m_view.x;
    y = m_view.y;
}

