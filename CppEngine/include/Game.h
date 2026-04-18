#pragma once
#include "gfx/VertexArray.h"
#include "gfx/VertexBuffer.h"
#include "gfx/Shader.h"
#include "IGame.h"
#include "Game/GameTypes.h"
#include "Game/Assets.h"
#include "Engine/ECS/Registry.h"
#include "ui/HUD.h"
#include <vector>
#include <cstdint>

class Camera;

class Game : public IGame {
public:
    void onInit(Engine& engine)          override;
    void onEvent(const SDL_Event& event)  override;
    void onUpdate(Engine& engine)          override;
    void onRender(Engine& engine)          override;
    void onShutdown(Engine& engine) override;

private:
    ECS::Registry m_Registry;
    ECS::Entity m_PlayerHandle;

    ECS::Entity& createEntity(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b);

    void resetGame();
    void shootBullet();
    void updateBullets(float);
    void killPlayer();
    void buildLevel();
    void handleCollisions();

    void createCoin(float x, float y, float w, float h, int value);

    Camera* m_Camera = nullptr;

    GameAssets assets;

    HUD hud;
    TTF_Font* m_HudFont = nullptr;

    AnimationClip m_CoinClip;

    std::vector<ECS::Entity> m_Entities;
    std::vector<ECS::Entity> m_BulletEntities;
    std::vector<ECS::Entity> m_CoinEntities;
    GameState           m_State = GameState::Playing;
    bool                m_RequestedQuit = false;
    int score = 0;

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<Shader> m_Shader;
};