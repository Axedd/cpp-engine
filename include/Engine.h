#pragma once
#include <SDL.h>


class Engine {
public:
	Engine();
	~Engine();

	bool init(const char* title, int width, int height);
	void run();
	void shutdown();

	const float TARGET_FRAME_TIME = 1.0f / 144.0f;
	float getDeltaTime() const { return m_DeltaTime; }

	struct Box {
		float x, y;
		float w, h;
		float vx, vy;
	};
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
private:
	float m_DeltaTime;
	uint64_t m_LastCounter;
	uint64_t m_Frequency;
private:
	Box m_Player;
};

