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

	m_LastCounter = SDL_GetPerformanceCounter();
	m_Frequency = SDL_GetPerformanceFrequency();
	m_DeltaTime = 0.0f;
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

	m_Player.x = 100;
	m_Player.y = 100;
	m_Player.w = 50;
	m_Player.h = 50;

	m_Player.vx = 200.0f;
	m_Player.vy = 0.0f;

	m_Running = true;
	return true;
}


void Engine::run()
{
	while (m_Running)
	{
		uint64_t frameStart = SDL_GetPerformanceCounter();

		processInput();
		update();
		render();

		// Calculate how long frame took
		uint64_t frameEnd = SDL_GetPerformanceCounter();
		m_DeltaTime = (float)(frameEnd - frameStart) / (float)m_Frequency;

		// Frame limit (144 FPS)
		float target = TARGET_FRAME_TIME;

		if (m_DeltaTime < target)
		{
			float delayMs = (target - m_DeltaTime) * 1000.0f;
			SDL_Delay((Uint32)delayMs);

			// recalc final delta including sleep
			uint64_t finalEnd = SDL_GetPerformanceCounter();
			m_DeltaTime = (float)(finalEnd - frameStart) / (float)m_Frequency;
		}
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

	const Uint8* state = SDL_GetKeyboardState(NULL);

	m_Player.vx = 0;
	m_Player.vy = 0;

	float speed = 200.0f;

	if (state[SDL_SCANCODE_W]) m_Player.vy = -speed;
	if (state[SDL_SCANCODE_S]) m_Player.vy = speed;
	if (state[SDL_SCANCODE_A]) m_Player.vx = -speed;
	if (state[SDL_SCANCODE_D]) m_Player.vx = speed;
}

void Engine::update()
{
	m_Player.x += m_Player.vx * m_DeltaTime;
	m_Player.y += m_Player.vy * m_DeltaTime;
}

void Engine::render()
{
	// Clear screen
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);

	// Draw player
	SDL_Rect r;
	r.x = (int)m_Player.x;
	r.y = (int)m_Player.y;
	r.w = (int)m_Player.w;
	r.h = (int)m_Player.h;

	SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(m_Renderer, &r);

	// Present the final frame
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