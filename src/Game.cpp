#include "Game.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "Colors.h"
#include "Logger.h"
#include "MysteryShip.h"
#include "ParticleSystem.h"
#include "SpaceShip.h"
#include "states/MenuState.h"

namespace SpaceInvaders {

Game::Game() {
    InitWindow(ScreenWidth, ScreenHeight, "Raylib Space Invaders!");

    InitAudioDevice();
    SetExitKey(KEY_NULL);

    try {
        Resources->LoadTextures("Graphics");
        Resources->LoadSounds("Sounds/Effects");
        Resources->LoadFonts("Fonts");
        Resources->LoadShaders("Graphics/Shaders");
    } catch (const std::runtime_error &e) {
        LogError(e.what());
        std::terminate();
    }

    LoadHighScore();
    SetRandomSeed(static_cast<int32_t>(GetTime()));

    m_bloomRenderer.Init(ScreenWidth, ScreenHeight);
    m_bloomRenderer.SetThreshold(0.60f);
    m_bloomRenderer.SetKnee(0.20f);
    m_bloomRenderer.SetBlurRadius(4.0f);
}

Game::~Game() {
    SaveHighScore();
    Resources.reset(); // Resources need to be unloaded before CloseWindow() is called
    m_player.reset();
    m_mystery.reset();
    m_alienLasers.clear();
    for (auto &barrier : m_barriers) { barrier.reset(); }
    for (auto &alien : m_aliens) { alien.reset(); }
    CloseAudioDevice();
    CloseWindow();
}

void
Game::Run() {
    const auto f = Resources->Get<Font>("monogram.ttf");
    if (!f.has_value()) {
        LogError("Unable to load font: monogram.ttf");
        return;
    }
    m_font = f.value();

    StateManager->PushState(std::make_unique<MenuState>(), this);

    while (!WindowShouldClose() && !m_shouldExit && !StateManager->IsEmpty()) {
        StateManager->HandleInput(this);
        StateManager->Update(this);

        // m_bloomRenderer.ResizeIfNeeded(GetScreenWidth(), GetScreenHeight());
        // m_bloomRenderer.BeginScene();
        // ClearBackground(Colors::Gray);
        // StateManager->Draw(this);
        // m_bloomRenderer.EndScene();
        //
        // m_bloomRenderer.Apply();
        //
        // BeginDrawing();
        // ClearBackground(Colors::Black);
        // m_bloomRenderer.Composite(1.0f);
        // EndDrawing();

        BeginDrawing();
        ClearBackground(Colors::Gray);
        StateManager->Draw(this);
        EndDrawing();
    }
}

void
Game::Update() {
    if (GetAliensLeft() <= 0) {
        // TODO:  Make this a state. Implement some sort of delay, and possibly aliens marching in animation
        m_level++;
        m_alienLasers.clear();

        for (auto &barrier: m_barriers) { barrier.reset(); }
        for (auto &alien: m_aliens) { alien.reset(); }

        try {
            m_player = std::make_unique<SpaceShip>();
            m_mystery = std::make_unique<MysteryShip>();

            CreateAliens();
            CreateBarriers();
        } catch (const std::runtime_error &e) {
            LogError(e.what());
            std::terminate();
        }
    }

    m_mystery->Update();

    for (const auto &laser : m_alienLasers) { laser->Update(); }
    UpdateVisualEffects();

    // ***** Everything below here only happens if the game is not over.
    if (m_gameOver) { return; }

    m_player->Update();
    MoveAliens();

    int chosen = -1;
    int aliveCount = 0;
    for (int i = 0; i < static_cast<int>(m_aliens.size()); ++i) {
        if (m_aliens[i]->GetActive()) {
            ++aliveCount;
            if (GetRandomValue(1, aliveCount) == 1) {
                chosen = i;
            }
        }
    }
    if (chosen != -1) {
        m_aliens[chosen]->FireLaser();
    }
}

void
Game::UpdateVisualEffects() {
    std::erase_if(m_alienLasers, [](auto& laser) { return !laser->GetActive(); });

    ParticleManager->Update(GetFrameTime());
}

void
Game::Draw() const {
    if (const auto background = Resources->Get<Texture2D>("background.png"); background.has_value()) {
        const Texture2D &bgTexture = background.value();

        const Rectangle source = {0, 0, static_cast<float>(bgTexture.width), static_cast<float>(bgTexture.height)};
        const Rectangle dest = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

        DrawTexturePro(bgTexture, source, dest, {0, 0}, 0.0f, WHITE);
    }
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(Colors::Black, 0.50f));

    if (m_player) m_player->Draw();
    if (m_mystery) m_mystery->Draw();

    // Draw all the things...
    for (const auto &barrier: m_barriers) { barrier->Draw(); }
    for (const auto &alien: m_aliens) { alien->Draw(); }
    for (const auto &laser: m_alienLasers) { laser->Draw(); }

    ParticleManager->Draw();

    // // Draw Vignette last so that it fades everything
    if (const auto vignette = Resources->Get<Texture2D>("vignette.png"); vignette.has_value()) {
        const Texture2D &vTexture = vignette.value();

        const Rectangle source = {0, 0, static_cast<float>(vTexture.width), static_cast<float>(vTexture.height)};
        const Rectangle dest = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

        DrawTexturePro(vTexture, source, dest, {0, 0}, 0.0f, ColorAlpha(Colors::White, 0.8f));
    }
}

void
Game::DrawUI() {
    DrawLineEx({ScreenPadding / 2, GetGroundLevel()},
        {GetScreenWidth() - ScreenPadding / 2.0f, GetGroundLevel()}, 3, Colors::Yellow);

    DrawTextEx(m_font, std::format("LEVEL {:02d}", m_level).c_str(),
        { GetScreenWidth() - 230.0f, GetScreenHeight() - 60.0f }, FontSize, FontSpacing, Colors::Yellow);

    for (uint8_t i = 0; i < m_playerLives; i++) {
        DrawTextureV(m_player->GetTexture(), {m_player->GetTexture().width + 65.0f * i, GetScreenHeight() - 55.0f}, WHITE);
    }

    DrawTextEx(m_font, "SCORE", {50, 15}, FontSize, FontSpacing, Colors::Yellow);
    const auto scoreText = std::format("{:05d}", m_score);
    DrawTextEx(m_font, scoreText.c_str(), {50, 40}, FontSize, FontSpacing, Colors::Yellow);

    DrawTextEx(m_font, "HIGH-SCORE", {GetScreenWidth() - 230.0f, 15}, FontSize, FontSpacing, Colors::Yellow);
    const auto highScoreText = std::format("{:05d}", m_highScore);
    DrawTextEx(m_font, highScoreText.c_str(), {GetScreenWidth() - 158.0f, 40}, FontSize, FontSpacing, Colors::Yellow);
}

void
Game::HandleInput() const {
    if (m_player) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            m_player->MoveLeft();
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            m_player->MoveRight();
        }
        if (IsKeyDown(KEY_SPACE)) {
            m_player->FireLaser();
        }
    }
}

void
Game::Reset() {
    m_gameOver = false;
    m_score = 0;
    m_playerLives = PlayerLives;
    m_alienLasers.clear();

    for (auto &barrier: m_barriers) { barrier.reset(); }
    for (auto &alien: m_aliens) { alien.reset(); }

    try {
        m_player = std::make_unique<SpaceShip>();
        m_mystery = std::make_unique<MysteryShip>();

        CreateAliens();
        CreateBarriers();
    } catch (const std::runtime_error &e) {
        LogError(e.what());
        std::terminate();
    }
}

void
Game::CheckCollisions() {
    CheckPlayerCollisions();
    CheckAlienCollisions();
}

/**
 * @brief Checks for and handles collisions between the player's lasers and various game entities.
 *
 * This method determines whether any of the player's active lasers collide
 * with aliens, barriers, alien lasers, or the mystery ship, and performs
 * the corresponding actions based on the collision:
 * - Player lasers colliding with aliens will destroy both the laser and the alien,
 *   and update the score based on the type of the alien.
 * - Player lasers colliding with barriers will either damage the barrier or
 *   specific cells of the barrier if applicable, and destroy the laser.
 * - Player lasers colliding with alien lasers will destroy both lasers
 *   and reward points to the player.
 * - Player lasers colliding with the mystery ship will destroy both
 *   the laser and the mystery ship, and award points to the player.
 *
 * This function ensures proper interaction between the player's lasers
 * and other game entities, updating the game state accordingly.
 */
void
Game::CheckPlayerCollisions() {
    if (!m_player) { return; }

    for (auto &laser : m_player->GetLasers()) {
        if (const auto entity = laser.CollidesWithAny(m_aliens); entity) {
            const auto alien = std::dynamic_pointer_cast<Alien>(entity);
            laser.Explode(false);
            alien->Explode();
            IncrementScore(alien->GetType() * 100);
            continue;
        }

        if (const auto entity = laser.CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = laser.CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                barrier->Damage(laser);
                laser.Explode(true);
                continue;
            }
        }

        if (const auto entity = laser.CollidesWithAny(m_alienLasers); entity) {
            const auto aLaser = std::dynamic_pointer_cast<Laser>(entity);
            laser.Explode(true);
            aLaser->Explode(true);
            IncrementScore(1000);
        }

        if (laser.CollidesWith(*m_mystery)) {
            laser.Explode(false);
            m_mystery->Explode();
            IncrementScore(500);
        }
    }
}

/**
 * @brief Handles collisions between alien entities, alien lasers, the player, and barriers.
 *
 * This function checks for and resolves multiple types of collisions in the game, ensuring accurate
 * interaction between alien lasers, barriers, the player, and aliens themselves. Collisions are checked
 * and resolved in a priority-based manner:
 *
 * - Alien lasers are checked for collisions with the player. If a collision occurs, the laser is destroyed,
 *   and the player's life is decremented if the player dies.
 * - Alien lasers are checked for collisions with barriers. The function first checks the bounding rectangle
 *   of each barrier for efficiency before processing individual cells within the barrier. If a collision
 *   occurs with a barrier cell, the barrier is damaged at the collided cell, and the laser is destroyed.
 * - Aliens are checked for collisions with barriers. Similar to laser-barrier interaction, the bounding
 *   rectangle is checked first, followed by the individual cells, which are damaged accordingly.
 * - Aliens are checked for collisions with the player. If a collision occurs, the player's life is decremented
 *   if the player dies.
 *
 * This function ensures efficient calculations by selectively narrowing down collision checks to relevant objects
 * and by appropriately resolving interactions between entities.
 */
void
Game::CheckAlienCollisions() {
    for (const auto &laser : m_alienLasers) {
        if (m_player && laser->CollidesWith(*m_player)) {
            laser->Explode(false);
            if (m_player->Die()) {
                DecrementPlayerLives();
            }
        }

        if (const auto entity = laser->CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = laser->CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                barrier->Damage(*laser);
                laser->Explode(true);
            }
        }
    }

    for (const auto &alien : m_aliens) {
        if (const auto entity = alien->CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = alien->CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                const auto cell = std::dynamic_pointer_cast<CellRect>(cellEntity);
                barrier->Damage(cell->GetPosition());
            }
        }

        if (m_player) {
            if (const auto entity = m_player->CollidesWithAny(m_aliens); entity) {
                if (m_player->Die()) {
                    DecrementPlayerLives();
                }
            }
        }
    }
}

void
Game::DecrementPlayerLives() {
    m_playerLives--;
    if (m_playerLives <= 0) {
        m_gameOver = true;
    }
}

void
Game::IncrementScore(const int16_t score) {
    m_score += score;
    if (m_score > m_highScore) {
        m_highScore = m_score;
    }
}

void
Game::CreateBarriers() {
    constexpr float gap = Barrier::BarrierWidth * 1.5f;
    constexpr float totalWidth = 4 * Barrier::BarrierWidth + gap * 3;
    const auto xOff = (GetScreenWidth() - totalWidth) / 2.0;

    for (int8_t i = 0; i < 4; i++) {
        const float xPos = xOff + i * Barrier::BarrierWidth + i * gap;
        m_barriers[i] = std::make_unique<Barrier>(Vector2 { xPos, GetGroundLevel() - 100.0f });
    }
}

/**
 * @brief Creates and positions a grid of alien entities within the game.
 *
 * This method performs the following operations:
 * - Initializes each alien entity in the grid, assigning a type based on its row position.
 * - Determines the maximum dimensions among all alien textures for uniform spacing.
 * - Calculates the total grid size and positions it horizontally centered on the screen.
 * - Arranges aliens within grid slots, ensuring each alien is centered in its respective slot.
 *
 * The positioning accounts for necessary gaps (horizontal and vertical spacing) between aliens,
 * and aligns the grid a fixed distance from the top of the screen.
 */
void
Game::CreateAliens() {
    float maxAlienWidth = 0.0f;
    float maxAlienHeight = 0.0f;

    for (size_t i = 0; i < m_aliens.size(); i++) {
        const auto row = i / AlienCols;
        uint8_t type = 3;
        if (row > 2) { type = 1; }
        else if (row > 0) { type = 2; }
        m_aliens[i] = std::make_shared<Alien>(Vector2 { 0, 0 }, type);

        const Texture2D &tex = m_aliens[i]->GetTexture();
        maxAlienWidth = std::max(maxAlienWidth, static_cast<float>(tex.width));
        maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(tex.height));
    }

    // Magic numbers...yeah yeah...I know
    constexpr float horizontalSpacing = 11.0f; // Gap between alien columns
    constexpr float verticalSpacing = 15.0f;   // Gap between alien rows

    const float totalGridWidth = (AlienCols * maxAlienWidth) + ((AlienCols - 1) * horizontalSpacing);

    const float startX = (GetScreenWidth() - totalGridWidth) / 2.0f;
    const float startY = std::min(110.0f + maxAlienHeight * m_level - 1, 325.0f);

    for (size_t i = 0; i < m_aliens.size(); i++) {
        const auto row = i / AlienCols;
        const auto col = i % AlienCols;

        const float slotX = startX + col * (maxAlienWidth + horizontalSpacing);
        const float slotY = startY + row * (maxAlienHeight + verticalSpacing);

        const Texture2D &tex = m_aliens[i]->GetTexture();
        const float centeredX = slotX + (maxAlienWidth - tex.width) / 2.0f;
        const float centeredY = slotY + (maxAlienHeight - tex.height) / 2.0f;

        m_aliens[i]->Move({ centeredX, centeredY });
    }

    Alien::ResetSpeed();
    m_alienMoveTrigger = GetAliensLeft();
}

void
Game::SaveHighScore() const {
    if (std::ofstream scoreFile("highscore.txt"); scoreFile.is_open()) {
        scoreFile << m_highScore;
    } else {
        println(std::cerr, "Unable to open highscore.txt for writing");
    }
}

void
Game::LoadHighScore() {
    if (std::ifstream scoreFile("highscore.txt"); scoreFile.is_open()) {
        scoreFile >> m_highScore;
    } else {
        println(std::cerr, "Unable to open highscore.txt for reading");
    }
}

void
Game::AddAlienLaser(const std::shared_ptr<AlienLaser>& laser) {
    m_alienLasers.push_back(laser);
}

/**
 * @brief Updates the position and movement behavior of all aliens in the game.
 *
 * This method performs the following actions:
 * - Iterates through all active aliens and updates their individual states.
 * - Detects if any alien has moved beyond the horizontal screen boundaries.
 * - Adjusts the movement direction of aliens if boundary detection is triggered.
 * - Moves aliens downward collectively when required, adding a gap between rows.
 * - Dynamically increases alien movement speed based on the number of remaining aliens.
 *
 * The logic ensures that aliens stay within the screen boundaries and progress downward as expected,
 */
void
Game::MoveAliens() const {
    bool moveDown = false;
    float maxAlienHeight = 0;
    for (const auto &alien : m_aliens) {
        alien->Update();
        if (alien->GetActive() && (alien->GetPosition().x < ScreenPadding / 2.0f ||
            alien->GetPosition().x + alien->GetTexture().width > GetScreenWidth() - ScreenPadding / 2)) {
            moveDown = true;
        }

        maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(alien->GetTexture().height));
    }

    const auto aliensLeft = GetAliensLeft();
    if (aliensLeft > 0 && (static_cast<float>(aliensLeft) / m_alienMoveTrigger) * 100.0f < 90.0f) {
        Alien::StepUpSpeed();
        m_alienMoveTrigger = aliensLeft;
    }

    if (!moveDown) return;

    maxAlienHeight += 15.0f;   // Gap between alien rows
    for (const auto &alien : m_aliens) {
        alien->SetSpeed(-alien->GetSpeed());
        alien->Move({ alien->GetPosition().x + alien->GetSpeed(), alien->GetPosition().y + maxAlienHeight / 2 });
    }
}

}
