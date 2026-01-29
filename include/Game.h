#pragma once
#include "IGame.h"
#include "Camera.h"
#include "Game/GameTypes.h"
#include "Game/Assets.h"
#include <vector>

class Game : public IGame {
public:
    void onInit(Engine& engine)          override;
    void onEvent(const SDL_Event& event)  override;
    void onUpdate(Engine& engine)          override;
    void onRender(Engine& engine)          override;
    Coin& createCoin(float x, float y, float w, float h, int value = 1);

private:
    Entity& createEntity(float x, float y, float w, float h,
        float vx, float vy,
        std::uint8_t r, std::uint8_t g, std::uint8_t b);

    void resetGame();
    void shootBullet();
    void updateBullets(float);
    void removeBullets();
    void killPlayer();

    Camera* m_Camera;

    GameAssets assets;

    std::vector<Coin> coins;
    Player              m_Player{};
    std::vector<Entity> m_Entities;
    GameState           m_State = GameState::Playing;
    std::vector<Bullet> bullets;
    bool                m_RequestedQuit = false;
    int score = 0;
};