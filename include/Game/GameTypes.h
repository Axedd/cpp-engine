#pragma once
#include <cstdint>

// Axis-aligned bounding box (AABB) + optional velocity
struct Body {
	float x, y;
	float w, h;
	float vx = 0, vy = 0; // defaults to static
};

// Coins that can exist in the world (placed or dropped by killed enemies)
struct Coin {
	Body body;
	bool collected = false;

	// score value
	int value = 1;

	// Animation state
	int frame = 0;
	float animTimer = 0.0f;
};