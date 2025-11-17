#pragma once
#include <SDL.h>


class Engine {
public:
	Engine();
	~Engine();

	bool init(const char* title, int width, int height);
	void run();
	void shutdown();
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
};