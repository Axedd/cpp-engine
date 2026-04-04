#pragma once
#include <cstdint>


struct Transform {
    float x, y;
    float w, h;
};

struct Velocity {
    float vx, vy;
    bool isGrounded = false;
};

struct PlayerStats {
    int health;
    int lives;
    bool isAlive;
    float shootCooldown = 0.0f;
    Direction viewDir = Right; 
    bool isAiming = false;
};

struct Sprite {
    uint8_t r, g, b; 
};

struct Lifetime {
    float value;
};

struct CoinStats {
    int value;
    bool collected = false;
};