#include "Game.h"
#include "Engine.h"   
#include <iostream>

static constexpr float GRAVITY = 900.0f;
static constexpr float MOVE_SPEED = 200.0f;
static constexpr float JUMP_SPEED = -300.0f;

// Forward declarations for collision helpers
bool AABB(const Player& a, const Entity& b);
void resolveCollision(Player& player, const Entity& block);

// -------- IGame overrides --------

void Game::onInit(Engine& engine)
{
    m_Entities.clear();

    // ground
    createEntity(0, 500, 1000, 50, 0, 0, 0, 255, 255);

    // player
    m_Player = {
        200, 400, 50, 50,        // x,y,w,h
        0, 0,                    // vx,vy
        false,                   // isGrounded
        255, 0, 0,               // color
        100, 3, true             // health, lives, isAlive
    };

    m_State = GameState::Playing;
    m_RequestedQuit = false;
}

void Game::onEvent(const SDL_Event& e)
{
    if (e.type == SDL_QUIT) {
        m_RequestedQuit = true;
    }
}

void Game::onUpdate(Engine& engine)
{
    if (m_RequestedQuit) return;

    const Uint8* kb = SDL_GetKeyboardState(nullptr);
    float dt = engine.getDeltaTime();

    if (m_State == GameState::GameOver) {
        if (kb[SDL_SCANCODE_R]) resetGame();
        return;
    }

    // horizontal
    m_Player.vx = 0.0f;
    if (kb[SDL_SCANCODE_A]) m_Player.vx = -MOVE_SPEED;
    if (kb[SDL_SCANCODE_D]) m_Player.vx = MOVE_SPEED;

    // jump
    if (kb[SDL_SCANCODE_SPACE] && m_Player.isGrounded) {
        m_Player.vy = JUMP_SPEED;
        m_Player.isGrounded = false;
    }

    // gravity
    if (!m_Player.isGrounded)
        m_Player.vy += GRAVITY * dt;

    // integrate
    m_Player.x += m_Player.vx * dt;
    m_Player.y += m_Player.vy * dt;

    m_Player.isGrounded = false;

    // collisions
    for (Entity& e : m_Entities) {
        if (AABB(m_Player, e)) {
            resolveCollision(m_Player, e);
        }
    }

    // simple fell off world
    if (m_Player.y > 800.0f) {
        killPlayer();
    }
}

void Game::onRender(Engine& engine)
{
    SDL_Renderer* r = engine.getRenderer();

    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    // player
    SDL_Rect p{
        (int)m_Player.x,
        (int)m_Player.y,
        (int)m_Player.w,
        (int)m_Player.h
    };
    SDL_SetRenderDrawColor(r, m_Player.r, m_Player.g, m_Player.b, 255);
    SDL_RenderFillRect(r, &p);

    // entities
    for (const Entity& e : m_Entities) {
        SDL_Rect rect{ (int)e.x, (int)e.y, (int)e.w, (int)e.h };
        SDL_SetRenderDrawColor(r, e.r, e.g, e.b, 255);
        SDL_RenderFillRect(r, &rect);
    }

    SDL_RenderPresent(r);
}

// -------- Game private helpers --------

Entity& Game::createEntity(float x, float y, float w, float h,
    float vx, float vy,
    uint8_t r, uint8_t g, uint8_t b)
{
    Entity e{ x, y, w, h, vx, vy, r, g, b };
    m_Entities.push_back(e);
    return m_Entities.back();
}

void Game::resetGame()
{
    m_Entities.clear();
    createEntity(0, 500, 1000, 50, 0, 0, 0, 255, 255);

    m_Player = {
        200, 400, 50, 50,
        0, 0, false,
        255, 0, 0,
        100, 3, true
    };

    m_State = GameState::Playing;
    m_RequestedQuit = false;
}

void Game::killPlayer()
{
    if (!m_Player.isAlive) return;

    m_Player.lives--;
    m_Player.isAlive = false;

    if (m_Player.lives <= 0) {
        m_State = GameState::GameOver;
    }
    else {
        // simple respawn
        m_Player.x = 200;
        m_Player.y = 400;
        m_Player.vx = 0;
        m_Player.vy = 0;
        m_Player.isAlive = true;
        m_Player.isGrounded = false;
    }
}

bool AABB(const Player& a, const Entity& b)
{
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
        );
}

void resolveCollision(Player& player, const Entity& block)
{
    float px = player.x + player.w * 0.5f;
    float py = player.y + player.h * 0.5f;
    float bx = block.x + block.w * 0.5f;
    float by = block.y + block.h * 0.5f;

    float overlapX = (player.w * 0.5f + block.w * 0.5f) - std::fabs(px - bx);
    float overlapY = (player.h * 0.5f + block.h * 0.5f) - std::fabs(py - by);

    if (overlapX < overlapY) {
        // Horizontal push
        if (px < bx) player.x -= overlapX;
        else         player.x += overlapX;
        player.vx = 0.0f;
    }
    else {
        // Vertical push based on direction
        if (player.vy > 0.0f) {
            // Landing on top
            player.y = block.y - player.h;
            player.vy = 0.0f;
            player.isGrounded = true;
        }
        else if (player.vy < 0.0f) {
            // Hitting the bottom
            player.y = block.y + block.h;
            player.vy = 0.0f;
        }
        else {
            // Fallback (rare)
            if (py < by) player.y -= overlapY;
            else         player.y += overlapY;
        }
    }
}
