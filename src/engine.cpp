#include "Engine.h"
#include <iostream>


bool AABB(const Player& a, const Entity& b);
void resolveCollision(Player& player, const Entity& block);

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

	Entity enemy = createEntity(0, 0, 50, 50, 0.0, 0.0);

	m_Player = { 200, 400, 50, 50, 0, 0, false, 255, 0, 0 };

	createEntity(0, 500, 1000, 50, 0, 0, 0, 255, 255);

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

	float speed = 200.0f;

	if (state[SDL_SCANCODE_A]) m_Player.vx = -speed;
	if (state[SDL_SCANCODE_D]) m_Player.vx = speed;

	if (state[SDL_SCANCODE_SPACE] && m_Player.isGrounded) {
		m_Player.vy = -300.0f;
		m_Player.isGrounded = false;
	}

}

void Engine::update()
{
	std::cout << m_Player.isGrounded << "\n";

	// Apply gravity
	if (!m_Player.isGrounded)
	{
		m_Player.vy += GRAVITY * m_DeltaTime;
	}

	// Move player
	m_Player.x += m_Player.vx * m_DeltaTime;
	m_Player.y += m_Player.vy * m_DeltaTime;

	// Reset grounded (will be set true in collision)
	m_Player.isGrounded = false;

	// Collision
	for (Entity& e : m_Entities)
	{
		if (AABB(m_Player, e)) {
			resolveCollision(m_Player, e);
		}
	}
}

void Engine::render()
{
	// Clear screen
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_Renderer);

	// Draw player
	SDL_Rect p;
	p.x = (int)m_Player.x;
	p.y = (int)m_Player.y;
	p.w = (int)m_Player.w;
	p.h = (int)m_Player.h;
	SDL_SetRenderDrawColor(m_Renderer, m_Player.r, m_Player.g, m_Player.b, 255);
	SDL_RenderFillRect(m_Renderer, &p);

	// Draw entities
	for (const Entity& e : m_Entities) {
		SDL_Rect r;
		r.x = (int)e.x;
		r.y = (int)e.y;
		r.w = (int)e.w;
		r.h = (int)e.h;
		

		SDL_SetRenderDrawColor(m_Renderer, e.r, e.g, e.b, 255);
		SDL_RenderFillRect(m_Renderer, &r);
	}

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


Entity& Engine::createEntity(float x, float y, float w, float h,
	float vx, float vy,
	uint8_t r, uint8_t g, uint8_t b)
{
	Entity e;
	e.x = x;
	e.y = y;
	e.w = w;
	e.h = h;
	e.vx = vx;
	e.vy = vy;
	e.r = r;
	e.g = g;
	e.b = b;

	m_Entities.push_back(e);
	return m_Entities.back();
}

bool AABB(const Player& a, const Entity& b) {
	return (
		a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.h &&
		a.y + a.h > b.y
		);
}


void resolveCollision(Player& player, const Entity& block) {

	float px = player.x + player.w * 0.5f;
	float py = player.y + player.h * 0.5f;
	float bx = block.x + block.w * 0.5f;
	float by = block.y + block.h * 0.5f;

	float overlapX = (player.w * 0.5f + block.w * 0.5f) - fabs(px - bx);
	float overlapY = (player.h * 0.5f + block.h * 0.5f) - fabs(py - by);

	if (overlapX < overlapY) {
		// horizontal push
		if (px < bx) player.x -= overlapX;
		else         player.x += overlapX;
		player.vx = 0;
	}
	else {
		// vertical push based on direction
		if (player.vy > 0) {
			// landed
			player.y = block.y - player.h;
			player.vy = 0;
			player.isGrounded = true;
		}
		else if (player.vy < 0) {
			// hit ceiling
			player.y = block.y + block.h;
			player.vy = 0;
		}
		else {
			// fallback
			if (py < by) player.y -= overlapY;
			else         player.y += overlapY;
		}
	}
}