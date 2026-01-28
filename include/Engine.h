// Engine.h
#pragma once
#include <SDL.h>
#include <memory>
#include <cstdint>
#include "gfx/TextureManager.h"

class IGame; // forward declare
class TextureManager;

class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

    void setGame(IGame* game) { m_Game = game; }

    float         getDeltaTime() const { return m_DeltaTime; }
    SDL_Renderer* getRenderer()  const { return m_Renderer; }

    TextureManager& textures() { return *m_Textures; }

private:
    void processEvents();

private:
    SDL_Window* m_Window = nullptr;
    SDL_Renderer* m_Renderer = nullptr;
    bool          m_Running = false;

    float     m_DeltaTime = 0.0f;
    std::uint64_t m_LastCounter = 0;
    std::uint64_t m_Frequency = 1;

    IGame* m_Game = nullptr;   // the currently running game

    std::unique_ptr<TextureManager> m_Textures;
};
