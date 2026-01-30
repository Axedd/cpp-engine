#pragma once
#include <cstdint>

enum class GameState {
	Playing,
	GameOver
};

enum Direction {
	Left,
	Right
};

struct Entity {
	float x, y;
	float w, h;
	float vx, vy;

	uint8_t r, g, b;
};

struct Player {
	float x, y;
	float w, h;

	float vx, vy; // velocity
	bool isGrounded; // ground flag for jumps

	uint8_t r, g, b;

	int health;
	int lives;
	bool isAlive;
	float shootCooldown = 0.2f;

	Direction viewDir = Right;
	bool isAiming;
};

struct Bullet {
	Entity body;
	float damage;
	Direction dir;
	float lifetime = 0.5f;
};

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

