#pragma once
#include "GameTypes.h"


inline Body ToBody(const Player& p)
{
    return Body{ p.x, p.y, p.w, p.h, p.vx, p.vy };
}

inline Body ToBody(const Entity& e)
{
    return Body{ e.x, e.y, e.w, e.h, e.vx, e.vy };
}

inline bool AABB(const Body& a, const Body& b)
{
    return (a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y);
}

inline bool AABB(const Player& a, const Entity& b)
{
    return AABB(ToBody(a), ToBody(b));
}

inline bool AABB(const Player& a, const Coin& c)
{
    return AABB(ToBody(a), c.body);
}