#include "Game.h"
#include "Engine.h"   
#include "Game/Collision.h"
#include <iostream>
#include <cmath>
#include <algorithm>


static constexpr float GRAVITY = 900.0f;
static constexpr float MOVE_SPEED = 200.0f;
static constexpr float JUMP_SPEED = -300.0f;


static void resolveCollision(Player& player, const Entity& block);
static void renderCoin(SDL_Renderer* r, const Coin& c, const SpriteSheet& sheet, int camX, int camY);

// -------- IGame overrides --------

void Game::onInit(Engine& engine)
{
    m_Entities.clear();

    SDL_Renderer* r = engine.getRenderer();

        
    // Coin
    assets.coin.tex = engine.textures().load("coin", "assets/coin.png");
    assets.coin.frameW = 32;
    assets.coin.frameH = 32;
    assets.coin.frames = 1;
    assets.coin.fps = 0.0f;
    assets.coin.loop = false;

    // Create coins
    createCoin(100, 400, 32, 32, 1);
    createCoin(200, 400, 32, 32, 1);


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

    m_Camera = new Camera(1024, 768, 2000, 1500);


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

    m_Player.isAiming = false;

    m_Camera->x = (m_Player.x + m_Player.w / 2) - (1024 / 2);
    m_Camera->y = (m_Player.y + m_Player.h / 2) - (768 / 2);

    m_Camera->update(m_Player.x, m_Player.y, m_Player.w, m_Player.h, dt);

    if (m_State == GameState::GameOver) {
        if (kb[SDL_SCANCODE_R]) resetGame();
        return;
    }

    // horizontal
    m_Player.vx = 0.0f;
    if (kb[SDL_SCANCODE_A]) {
        m_Player.vx = -MOVE_SPEED;
        m_Player.viewDir = Left;
    }
    if (kb[SDL_SCANCODE_D]) {
        m_Player.vx = MOVE_SPEED;
        m_Player.viewDir = Right;
    }

    // jump
    if (kb[SDL_SCANCODE_SPACE] && m_Player.isGrounded) {
        m_Player.vy = JUMP_SPEED;
        m_Player.isGrounded = false;
    }

    // m_Camera->shake(20.0f, 5.0f);

    // gravity
    if (!m_Player.isGrounded)
        m_Player.vy += GRAVITY * dt;

    // Shoot
    if (kb[SDL_SCANCODE_E] && m_Player.shootCooldown == 0) {
        shootBullet();
        m_Player.shootCooldown = 0.2f;
    }

    if (kb[SDL_SCANCODE_K]) m_Player.isAiming = true;

    // integrate
    m_Player.x += m_Player.vx * dt;
    m_Player.y += m_Player.vy * dt;

    m_Player.shootCooldown -= dt;
    if (m_Player.shootCooldown < 0) m_Player.shootCooldown = 0;


    m_Player.isGrounded = false;

    updateBullets(dt);

    // collisions
    for (Entity& e : m_Entities) {
        if (AABB(m_Player, e)) resolveCollision(m_Player, e);
    }

    for (Coin& c : coins) {
        if (!c.collected && AABB(m_Player, c)) {
            c.collected = true;
            std::cout << "Coin pick-up\n";
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

    int camX = m_Camera->getRenderX();
    int camY = m_Camera->getRenderY();

    // 1. Render Player (Subtract camera offset)
    SDL_Rect p{ (int)m_Player.x - camX,
                (int)m_Player.y - camY,
                (int)m_Player.w,
                (int)m_Player.h };
    SDL_SetRenderDrawColor(r, m_Player.r, m_Player.g, m_Player.b, 255);
    SDL_RenderFillRect(r, &p);

    // 2. Render Entities (Subtract camera offset)
    for (const Entity& e : m_Entities) {
        SDL_Rect rect{ (int)e.x - camX,
                       (int)e.y - camY,
                       (int)e.w,
                       (int)e.h };
        SDL_SetRenderDrawColor(r, e.r, e.g, e.b, 255);
        SDL_RenderFillRect(r, &rect);
    }

    // 3. Render Aiming UI/Effect (Subtract camera offset)
    if (m_Player.isAiming) {
        float spawnX = (m_Player.viewDir == Right) ? m_Player.x + m_Player.w : m_Player.x - m_Player.w;
        float spawnY = m_Player.y + m_Player.h / 2 - 10;

        // Note: Subtracting m_Camera.x/y here so the dots stay relative to the world
        SDL_Rect rect{ (int)spawnX - m_Camera->x, (int)spawnY - m_Camera->y, 10, 10 };
        SDL_Rect rect2{ (int)spawnX + 20 - m_Camera->x, (int)spawnY - m_Camera->y, 10, 10 };
        SDL_Rect rect3{ (int)spawnX + 40 - m_Camera->x, (int)spawnY - m_Camera->y, 10, 10 };

        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &rect);
        SDL_RenderFillRect(r, &rect2);
        SDL_RenderFillRect(r, &rect3);
    }

    // 4. Render Bullets (Subtract camera offset)
    for (const Bullet& b : bullets) {
        Entity body = b.body;
        SDL_Rect rect{ (int)body.x - m_Camera->x,
                       (int)body.y - m_Camera->y,
                       (int)body.w,
                       (int)body.h };
        SDL_SetRenderDrawColor(r, body.r, body.g, body.b, 255);
        SDL_RenderFillRect(r, &rect);
    }

    for (const auto& c : coins)
        if (!c.collected)
            renderCoin(r, c, assets.coin, camX, camY);

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

static void renderCoin(SDL_Renderer* r, const Coin& c, const SpriteSheet& sheet, int camX, int camY)
{
    SDL_Rect src{ c.frame * sheet.frameW, 0, sheet.frameW, sheet.frameH };
    SDL_Rect dst{
        (int)c.body.x - camX,
        (int)c.body.y - camY,
        (int)c.body.w,
        (int)c.body.h
    };
    SDL_RenderCopy(r, sheet.tex, &src, &dst);
}

Coin& Game::createCoin(float x, float y, float w, float h, int value)
{
    Coin c{};
    c.body.x = x;
    c.body.y = y;
    c.body.w = w;
    c.body.h = h;
    c.value = value;

    coins.push_back(c);
    return coins.back();
}


void Game::shootBullet() {
    float spawnX = (m_Player.viewDir == Right)
        ? m_Player.x + m_Player.w
        : m_Player.x - 20;

    float spawnY = m_Player.y + m_Player.h / 2 - 10;

    Entity e = Entity(spawnX, spawnY, 20, 20, 500, 0, 255, 0, 0);
    Bullet bulletEntity = { e , 0.2f, m_Player.viewDir};
    bullets.push_back(bulletEntity);
}

void Game::updateBullets(float dt)
{
    for (auto& b : bullets) {
        b.lifetime -= dt;

        if (b.dir == Right) b.body.x += b.body.vx * dt;
        else               b.body.x -= b.body.vx * dt;
    }

    removeBullets(); // Call once (prevents invalidation of b in iterator)
}

void Game::removeBullets() {
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return b.lifetime <= 0; }),
        bullets.end()
    );
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

// If collision is detected (AABB function) 
// We correct the entity in space with resolveCollision()
static void resolveCollision(Player& player, const Entity& block)
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

