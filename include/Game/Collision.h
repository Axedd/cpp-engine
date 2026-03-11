#pragma once
#include "Game/Components/Components.h" 

// Internal calculation
inline bool AABB(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2);
}

// ECS version: Checks two transforms
inline bool AABB(const Transform& a, const Transform& b) {
    return AABB(a.x, a.y, a.w, a.h, b.x, b.y, b.w, b.h);
}

// Resolve Collision: Move player away from a block
inline void resolveCollision(Transform& transform, Velocity& vel, const Transform& block) {
    float px = transform.x + transform.w * 0.5f;
    float py = transform.y + transform.h * 0.5f;
    float bx = block.x + block.w * 0.5f;
    float by = block.y + block.h * 0.5f;

    float overlapX = (transform.w * 0.5f + block.w * 0.5f) - (px > bx ? px - bx : bx - px);
    float overlapY = (transform.h * 0.5f + block.h * 0.5f) - (py > by ? py - by : by - py);

    if (overlapX < overlapY) {
        // Horizontal push
        if (px < bx) transform.x -= overlapX;
        else         transform.x += overlapX;
        vel.vx = 0.0f;
    }
    else {
        // +/-Vertical push
        if (vel.vy > 0.0f) { // Landing
            transform.y = block.y - transform.h;
            vel.vy = 0.0f;
            vel.isGrounded = true;
        }
        else if (vel.vy < 0.0f) { // Roof collision
            transform.y = block.y + block.h;
            vel.vy = 0.0f;
        }
    }
}