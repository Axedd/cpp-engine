// engine.cpp
#include <glad/glad.h>
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

    // tell SDL which version of OpenGL (3.3 Core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    // activate double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_Window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL
    );

    // create OpenGL context (for GPU memory)
    m_GLContext = SDL_GL_CreateContext(m_Window);
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "ERROR: Could not initialize GLAD!" << std::endl;
        return false;
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

    if (!m_Window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        return false;
    }


    m_Frequency = SDL_GetPerformanceFrequency();
    m_LastCounter = SDL_GetPerformanceCounter();
    m_DeltaTime = 0.0f;
    m_Running = true;

    m_Textures = std::make_unique<TextureManager>();

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
            // Clear the screen
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_Game->onUpdate(*this);
            m_Game->onRender(*this);

            SDL_GL_SwapWindow(m_Window);
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
        m_Game = nullptr; // optional: avoid double cleanup
    }

    if (m_Textures) {
        m_Textures->clear();
        m_Textures.reset();
    }

    TTF_Quit();

    if (m_GLContext) {
        SDL_GL_DeleteContext(m_GLContext);
        m_GLContext = nullptr;
    }

    if (m_Window) {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }



    SDL_Quit();
}
