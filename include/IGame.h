#pragma once
#include <SDL.h>
#include "Game/GameTypes.h"
class Engine;


class IGame {
public:
	virtual ~IGame() = default;

	virtual void onInit(Engine& engine) = 0;
	virtual void onEvent(const SDL_Event& e) = 0;
	virtual void onUpdate(Engine& engine) = 0;
	virtual void onRender(Engine& engine) = 0;

	virtual void onShutdown(Engine& engine) {};
};