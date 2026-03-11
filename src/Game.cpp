#include "Game.h"
#include "Engine.h"   
#include "Game/Collision.h"
#include "Game/Components/Components.h"
#include "Camera.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "gfx/TextureManager.h"


// physics
static constexpr float GRAVITY = 900.0f;
static constexpr float MOVE_SPEED = 200.0f;
static constexpr float JUMP_SPEED = -300.0f;


static void renderCoin(SDL_Renderer* r, ECS::Entity& c, const SpriteSheet& sheet, int camX, int camY);

// -------- IGame overrides --------

void Game::onInit(Engine& engine)
{
    SDL_Renderer* r = engine.getRenderer();

    AnimationClip coinClip;

    m_PlayerHandle = m_Registry.createEntity();

    // Add components
    m_PlayerHandle.add<Transform>(400.0f, 400.0f, 50.0f, 50.0f);
    m_PlayerHandle.add<Velocity>(0.0f, 0.0f);
    m_PlayerHandle.add<PlayerStats>(100, 3, true);

        
    // Coin
    assets.coin.tex = engine.textures().load("coin", "assets/coin_anim.png");
    assets.coin.frameW = 32;
    assets.coin.frameH = 32;

    m_CoinClip.startFrame = 0;
    m_CoinClip.frameCount = 6; // or 7
    m_CoinClip.fps = 9.0f;
    m_CoinClip.loop = true;


    // Ensure load/create once
    if (!m_Camera) {
        m_Camera = new Camera(1024, 768, 2000, 1500);
    }

    if (!m_HudFont) {
        m_HudFont = TTF_OpenFont("assets/fonts/Inter_24pt-Bold.ttf", 24);
        if (!m_HudFont) SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        hud.init(r, m_HudFont);
    }

    buildLevel();
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
    float dt = engine.getDeltaTime();
    const Uint8* kb = SDL_GetKeyboardState(nullptr);

    // ECS DATA
    auto& transform = m_PlayerHandle.get<Transform>();
    auto& vel = m_PlayerHandle.get<Velocity>();
    auto& stats = m_PlayerHandle.get<PlayerStats>();

    if (m_State == GameState::GameOver) {
        if (kb[SDL_SCANCODE_R]) resetGame();
        return;
    }

    stats.isAiming = false;
    m_Camera->update((int)transform.x, (int)transform.y, (int)transform.w, (int)transform.h, dt);

    for (auto& c : m_CoinEntities) {
        auto& cStats = c.get<CoinStats>();
        auto& cAnim = c.get<Animator>();
        if (!cStats.collected) cAnim.update(dt);
    }

    // Movement
    vel.vx = 0.0f;
    if (kb[SDL_SCANCODE_A]) { vel.vx = -MOVE_SPEED; stats.viewDir = Left; }
    if (kb[SDL_SCANCODE_D]) { vel.vx = MOVE_SPEED; stats.viewDir = Right; }

    if (kb[SDL_SCANCODE_SPACE] && vel.isGrounded) {
        vel.vy = JUMP_SPEED;
        vel.isGrounded = false;
    }

    if (!vel.isGrounded) vel.vy += GRAVITY * dt;

    transform.x += vel.vx * dt;
    transform.y += vel.vy * dt;

    // Shoot
    if (kb[SDL_SCANCODE_E] && stats.shootCooldown <= 0) {
        shootBullet();
        stats.shootCooldown = 0.2f;
    }
    if (kb[SDL_SCANCODE_K]) stats.isAiming = true;
    if (stats.shootCooldown > 0) stats.shootCooldown -= dt;

    updateBullets(dt);
    handleCollisions();

    // HUD data update
    HUDData data;
    data.score = score;
    data.health = stats.health;
    data.lives = stats.lives;
    hud.update(engine.getRenderer(), data);

    if (transform.y > 800.0f) killPlayer();
}

void Game::onRender(Engine& engine)
{
    SDL_Renderer* r = engine.getRenderer();

    auto& transform = m_PlayerHandle.get<Transform>();
    auto& stats = m_PlayerHandle.get<PlayerStats>();

    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    int camX = m_Camera->getRenderX();
    int camY = m_Camera->getRenderY();

    SDL_Rect p{ (int)transform.x - camX, (int)transform.y - camY, (int)transform.w, (int)transform.h };
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255); 
    SDL_RenderFillRect(r, &p);

    // Render entities and subtract camera offset
    for (auto& e : m_Entities) {
        auto& transform = e.get<Transform>();
        auto& sprite = e.get<Sprite>();

        SDL_Rect rect{ 
            (int)transform.x - camX, 
            (int)transform.y - camY, 
            (int)transform.w, 
            (int)transform.h 
        };
        SDL_SetRenderDrawColor(r, sprite.r, sprite.g, sprite.b, 255);
        SDL_RenderFillRect(r, &rect);
    }



    auto& pt = m_PlayerHandle.get<Transform>();
    SDL_Rect pRect{ (int)pt.x - camX, (int)pt.y - camY, (int)pt.w, (int)pt.h };
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderFillRect(r, &pRect);


    for (auto& b : m_BulletEntities) {
        auto& transform = b.get<Transform>();

        // for sprite asset (bullet)
        auto& sprite = b.get<Sprite>();

        SDL_Rect rect{
            (int)transform.x - camX,
            (int)transform.y - camY,
            (int)transform.w,
            (int)transform.h
        };

        // Brug farven fra Sprite-komponenten
        SDL_SetRenderDrawColor(r, sprite.r, sprite.g, sprite.b, 255);
        SDL_RenderFillRect(r, &rect);
    }
    for (auto& c : m_CoinEntities) {
        auto& t = c.get<Transform>();
        auto& anim = c.get<Animator>();

        int frame = anim.absoluteFrame();
        SDL_Rect src{ frame * assets.coin.frameW, 0, assets.coin.frameW, assets.coin.frameH };
        SDL_Rect dst{ (int)t.x - camX, (int)t.y - camY, (int)t.w, (int)t.h };

        SDL_RenderCopy(r, assets.coin.tex, &src, &dst);
    }

    for (auto& b : m_BulletEntities) {
        auto& t = b.get<Transform>();
        auto& s = b.get<Sprite>();
        SDL_Rect rect{ (int)t.x - camX, (int)t.y - camY, (int)t.w, (int)t.h };
        SDL_SetRenderDrawColor(r, s.r, s.g, s.b, 255);
        SDL_RenderFillRect(r, &rect);
    }


    for (auto& c : m_CoinEntities) {
        renderCoin(r, c, assets.coin, camX, camY);
    }

    hud.render(r, 1024, 768);

    SDL_RenderPresent(r);
}

void Game::onShutdown(Engine& engine)
{
    if (m_HudFont) {
        TTF_CloseFont(m_HudFont);
        m_HudFont = nullptr;
    }

    delete m_Camera;
    m_Camera = nullptr;

}

// -------- Game private helpers --------

void Game::buildLevel()
{
    // total reset of reigstry
    m_Registry.clear();

    m_Entities.clear();
    m_CoinEntities.clear();
    m_BulletEntities.clear();
    score = 0;

    // recreate player
    m_PlayerHandle = m_Registry.createEntity();

    // add player components
    m_PlayerHandle.add<Transform>(400.0f, 400.0f, 50.0f, 50.0f);
    m_PlayerHandle.add<Velocity>(0.0f, 0.0f);
    m_PlayerHandle.add<PlayerStats>(100, 3, true);

    // Build the level 

    createEntity(0, 600, 2000, 50, 100, 100, 100); // Long platform
    createEntity(200, 450, 200, 20, 100, 100, 100); // small floating platform

    // Create coins
    createCoin(300, 400, 32, 32, 10);
    createCoin(400, 400, 32, 32, 10);
    createCoin(500, 400, 32, 32, 10);

    // Optional: reset camera
    if (m_Camera) {
        // Make camera face player imediately 
        auto& t = m_PlayerHandle.get<Transform>();
        m_Camera->update((int)t.x, (int)t.y, (int)t.w, (int)t.h, 0.0f);
    }

    SDL_Log("Level build complete. Registry and entities reset.");
}
void Game::handleCollisions() {
    auto& pTrans = m_PlayerHandle.get<Transform>();
    auto& pVel = m_PlayerHandle.get<Velocity>();

    // Player against the floor
    for (auto& e : m_Entities) {
        auto& blockTrans = e.get<Transform>();
        if (AABB(pTrans, blockTrans)) {
            resolveCollision(pTrans, pVel, blockTrans);
        }
    }

    // Player against coins
    for (auto it = m_CoinEntities.begin(); it != m_CoinEntities.end();) {
        auto& coinTrans = it->get<Transform>();
        if (AABB(pTrans, coinTrans)) {
            score += 10;
            m_Registry.destroy(it->getId()); // Delete from ECS pools
            it = m_CoinEntities.erase(it);   // remove from the list of coins
        }
        else {
            ++it;
        }
    }
}

ECS::Entity& Game::createEntity(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b)
{
    ECS::Entity e = m_Registry.createEntity();

    // Add components data
    e.add<Transform>(x, y, w, h);
    e.add<Sprite>(r, g, b);

    m_Entities.push_back(e);
    return m_Entities.back();
}

static void renderCoin(SDL_Renderer* r, ECS::Entity& c, const SpriteSheet& sheet, int camX, int camY) {
    auto& t = c.get<Transform>();
    auto& anim = c.get<Animator>(); 

    int frame = anim.absoluteFrame();
    SDL_Rect src{ frame * sheet.frameW, 0, sheet.frameW, sheet.frameH };
    SDL_Rect dst{ (int)t.x - camX, (int)t.y - camY, (int)t.w, (int)t.h };
    SDL_RenderCopy(r, sheet.tex, &src, &dst);
}

void Game::createCoin(float x, float y, float w, float h, int value) {
    ECS::Entity c = m_Registry.createEntity();

    c.add<Transform>(x, y, w, h);
    c.add<CoinStats>(value);

    // run coin animation
    auto& anim = c.add<Animator>();
    anim.play(&m_CoinClip, true);

    m_CoinEntities.push_back(c);
}

void Game::shootBullet() {
    auto& pTrans = m_PlayerHandle.get<Transform>();
    auto& pStats = m_PlayerHandle.get<PlayerStats>();

    float spawnX = (pStats.viewDir == Right) ? pTrans.x + pTrans.w : pTrans.x - 20;

    // Bullets are made as ECS entities
    ECS::Entity b = m_Registry.createEntity();
    b.add<Transform>(spawnX, pTrans.y + 15, 10.0f, 10.0f);
    b.add<Velocity>((pStats.viewDir == Right ? 500.0f : -500.0f), 0.0f);
    b.add<Lifetime>(1.5f);
    b.add<Sprite>(255, 255, 0);

    m_BulletEntities.push_back(b);
}
void Game::updateBullets(float dt) {
    // Iterator to safely remove bullets when going over the list
    for (auto it = m_BulletEntities.begin(); it != m_BulletEntities.end();) {
        
        // Get components from the registry (it is a pointer to an ECS::Entity)
        auto& l = it->get<Lifetime>();
        auto& t = it->get<Transform>();
        auto& v = it->get<Velocity>();

        // Update the data
        l.value -= dt;            
        t.x += v.vx * dt;         // move along x-axis
        t.y += v.vy * dt;         // move along y-axis

        // check if bullets needs to be destroyed
        if (l.value <= 0.0f) {

            m_Registry.destroy(it->getId()); 
            
            it = m_BulletEntities.erase(it); 
        } 
        else {

            ++it;
        }
    }
}


void Game::resetGame()
{
    buildLevel();
    m_State = GameState::Playing;
    m_RequestedQuit = false;
}

void Game::killPlayer() {
    // Get data directly from the source
    auto& stats = m_PlayerHandle.get<PlayerStats>();
    auto& transform = m_PlayerHandle.get<Transform>();

    stats.lives--;
    if (stats.lives <= 0) {
        m_State = GameState::GameOver;
    }
    else {
        transform.x = 400; // Reset position
        transform.y = 400;
    }
}
