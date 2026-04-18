#include "Game.h"
#include "Engine.h"   
#include "Game/Collision.h"
#include "Game/Components/Components.h"
#include "Camera.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "gfx/TextureManager.h"
#include "gfx/Vertex.h"


// physics
static constexpr float GRAVITY = 900.0f;
static constexpr float MOVE_SPEED = 200.0f;
static constexpr float JUMP_SPEED = -600.0f;


static void renderCoin(SDL_Renderer* r, ECS::Entity& c, const SpriteSheet& sheet, int camX, int camY);

// -------- IGame overrides --------

void Game::onInit(Engine& engine)
{
    m_Shader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    Vertex vertices[] = {
        { -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f },
        {  0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f }
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));

    unsigned int stride = sizeof(Vertex);
    m_VAO->addBuffer(*m_VBO, 0, 3, stride, offsetof(Vertex, x)); // Pos
    m_VAO->addBuffer(*m_VBO, 1, 2, stride, offsetof(Vertex, u)); // UV
    m_VAO->addBuffer(*m_VBO, 2, 4, stride, offsetof(Vertex, r)); // Color

    m_PlayerHandle = m_Registry.createEntity();

    // Add components
    m_PlayerHandle.add<Transform>(400.0f, 400.0f, 50.0f, 50.0f);
    m_PlayerHandle.add<Velocity>(0.0f, 0.0f);
    m_PlayerHandle.add<PlayerStats>(100, 3, true);

        
    // Coin
    assets.coin.textureID = engine.textures().load("coin", "coin.asset");
    assets.coin.frameW = 32;
    assets.coin.frameH = 32;

    m_CoinClip.startFrame = 0;
    m_CoinClip.frameCount = 6;
    m_CoinClip.fps = 9.0f;
    m_CoinClip.loop = true;


    // Ensure load/create once
    if (!m_Camera) {
        m_Camera = new Camera(1024, 768, 2000, 1500);
    }

    if (!m_HudFont) {
        m_HudFont = TTF_OpenFont("assets/fonts/Inter_24pt-Bold.ttf", 24);
        if (!m_HudFont) {
            SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        }
        else {
            // hud.init(r, m_HudFont); <-- commented out for now
        }
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

    bool canJump = vel.isGrounded;
    vel.isGrounded = false;

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

    if (kb[SDL_SCANCODE_SPACE] && canJump) {
        vel.vy = JUMP_SPEED;
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
    //hud.update(NULL, data);

    if (transform.y > 800.0f) killPlayer();
}

void Game::onRender(Engine& engine)
{
    m_Shader->use();
    m_VAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Get camera offsets for world-to-screen translation
    int camX = m_Camera->getRenderX();
    int camY = m_Camera->getRenderY();

    // ------------
    // RENDER LOGIC
    // ------------
    // Note: All SDL_Render calls have been removed as they require SDL_Renderer
    // OpenGL rendering requires a shader program and vertex buffer (VAO/VBO)
    // the loops below are preserved so we can implement draw calls later

    // Player Rendering
    auto& pTransform = m_PlayerHandle.get<Transform>();
    // Future: Renderer::DrawQuad(pTransform.x - camX, pTransform.y - camY, pTransform.w, pTransform.h, color);

    // Static World Entities (Platforms)
    for (auto& e : m_Entities) {
        auto& t = e.get<Transform>();
        auto& s = e.get<Sprite>();
        // Future: Renderer::DrawQuad(t.x - camX, t.y - camY, t.w, t.h, {s.r, s.g, s.b});
    }

    // Bullets
    for (auto& b : m_BulletEntities) {
        auto& t = b.get<Transform>();
        auto& s = b.get<Sprite>();
        // Future: Renderer::DrawQuad(t.x - camX, t.y - camY, t.w, t.h, {s.r, s.g, s.b});
    }

    // Coins (Animated Sprites)
    for (auto& c : m_CoinEntities) {
        auto& t = c.get<Transform>();
        auto& anim = c.get<Animator>();

        int frame = anim.absoluteFrame();
        unsigned int textureID = assets.coin.textureID;

        // Future: Renderer::DrawSprite(textureID, t.x - camX, t.y - camY, t.w, t.h, frame, assets.coin.frameW);
    }

    // HUD
    // Note: hud.render(r, ...) is disabled until an OpenGL-compatible UI path is created.
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

static void renderCoin(ECS::Entity& c, const SpriteSheet& sheet, int camX, int camY) {
    auto& t = c.get<Transform>();
    auto& anim = c.get<Animator>();

    int frame = anim.absoluteFrame();

    // Calculate world-to-screen position
    int screenX = (int)t.x - camX;
    int screenY = (int)t.y - camY;

    // Use the OpenGL Texture ID from the sheet
    unsigned int textureID = sheet.textureID;

    // ----------------------------
    // OPENGL DRAW CALL PLACEHOLDER
    // ----------------------------
    // SDL_RenderCopy is removed as it is incompatible with the OpenGL pipeline
    // to render this we will need to pass the textureID, position, and 
    // current frame data to a shader program
    // Future implementation:
    // Renderer::DrawSprite(textureID, screenX, screenY, (int)t.w, (int)t.h, frame, sheet.frameW);
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
