#pragma once
#include <SDL.h>

class Engine;

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

class IGame {
public:
	virtual ~IGame() = default;

	virtual void onInit(Engine& engine) = 0;
	virtual void onEvent(const SDL_Event& e) = 0;
	virtual void onUpdate(Engine& engine) = 0;
	virtual void onRender(Engine& engine) = 0;
};