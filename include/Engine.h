#pragma once
#include <SDL.h>
#include <vector>


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
};

class Engine {
public:
	Engine();
	~Engine();

	bool init(const char* title, int width, int height);
	void run();
	void shutdown();

	const float TARGET_FRAME_TIME = 1.0f / 144.0f;
	float getDeltaTime() const { return m_DeltaTime; }
	
	Entity& createEntity(float x, float y, float w, float h,
		float vx = 0, float vy = 0,
		uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);


private:
	void processInput();
	void update();
	void render();

private:
	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;
	bool m_Running;

	int m_WindowWidth;
	int m_WindowHeight;

	float m_DeltaTime;
	uint64_t m_LastCounter;
	uint64_t m_Frequency;

	Player m_Player;
	std::vector<Entity> m_Entities;

	const float GRAVITY = 900.0f;
};


