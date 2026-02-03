#pragma once

struct SDL_Rect; // Forward declaration

class Camera {
public:
    Camera(int width, int height, int levelWidth, int levelHeight);

    void update(int targetX, int targetY, int targetW, int targetH, float deltaTime);
    void shake(float magnitude, float duration);

    int getRenderX() const { return (int)m_x + m_offsetX; }
    int getRenderY() const { return (int)m_y + m_offsetY; }

    // Return by value is fine with a forward declaration 
    SDL_Rect getRect() const;

    float x, y; // Kept as floats for smoother logic

private:
    float m_x, m_y;
    int m_w, m_h;
    int m_levelWidth, m_levelHeight;

    float m_shakeMagnitude = 0.0f;
    float m_shakeDuration = 0.0f;
    float m_shakeTimer = 0.0f;

    int m_offsetX = 0;
    int m_offsetY = 0;
};