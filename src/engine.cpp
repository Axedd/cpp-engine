#include "Engine.h"
#include <iostream>


// Constructor
Engine::Engine()
{
	// Avoid uninitialized pointers
	m_Window = nullptr;
	m_Renderer = nullptr;
	m_Running = false;
	m_WindowWidth = 0;
	m_WindowHeight = 0;
}

// Ensures shutdown is called on destruction
Engine::~Engine() {
	shutdown();
}


bool Engine::init(const char* title, int width, int height) {
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
		return false;
	}

	m_WindowWidth = width;
	m_WindowHeight = height;

	m_Window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		0
	);

	if (!m_Window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError << "\n";
		return false;
	}

	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

	if (!m_Renderer) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError << "\n";
		return false;
	}

	m_Running = true;
	return true;
}


void Engine::run() 
{
	while (m_Running) {
		processInput();
		update();
		render();
	}
}

void Engine::processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			m_Running = false;
		}
	}
}

void Engine::update()
{

}

void Engine::render()
{
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);


	SDL_RenderPresent(m_Renderer);
}

void Engine::shutdown()
{
	if (m_Renderer) {
		SDL_DestroyRenderer(m_Renderer);
	}

	if (m_Window) {
		SDL_DestroyWindow(m_Window);
	}

	SDL_Quit();
}