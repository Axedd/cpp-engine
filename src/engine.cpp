// engine.cpp
#include "Engine.h"
#include "IGame.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "gfx/TextureManager.h"

Engine::Engine() = default;

Engine::~Engine() {
    shutdown();
}

bool Engine::init(const char* title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return false;
    }

    m_Window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );

    if (!m_Window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        return false;
    }

    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_Renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        return false;
    
    }

    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << "\n";
        return false;
    }


    // Init SDL_Image
    const int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        std::cout << "IMG_Init error: " << IMG_GetError() << "\n";
        return false;
    }

    m_Textures = std::make_unique<TextureManager>(m_Renderer);

    m_Frequency = SDL_GetPerformanceFrequency();
    m_LastCounter = SDL_GetPerformanceCounter();
    m_DeltaTime = 0.0f;

    m_Running = true;

    // Call game init once SDL is ready
    if (m_Game) {
        m_Game->onInit(*this);
    }

    return true;
}


void Engine::processEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // let game see the event
        if (m_Game) {
            m_Game->onEvent(e);
        }

        // engine-level quit
        if (e.type == SDL_QUIT) {
            m_Running = false;
        }
    }
}

void Engine::run()
{
    while (m_Running) {
        std::uint64_t frameStart = SDL_GetPerformanceCounter();

        processEvents();

        if (m_Game) {
            m_Game->onUpdate(*this);
            m_Game->onRender(*this);
        }

        std::uint64_t frameEnd = SDL_GetPerformanceCounter();
        m_DeltaTime = static_cast<float>(frameEnd - frameStart)
            / static_cast<float>(m_Frequency);
    }
}

void Engine::shutdown()
{
    if (m_Game) {
        m_Game->onShutdown(*this);
        m_Game = nullptr; // optional: undgå double cleanup
    }

    if (m_Textures) {
        m_Textures->clear();
        m_Textures.reset();
    }

    IMG_Quit();
    TTF_Quit();

    if (m_Renderer) {
        SDL_DestroyRenderer(m_Renderer);
        m_Renderer = nullptr;
    }
    if (m_Window) {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }



    SDL_Quit();
}
