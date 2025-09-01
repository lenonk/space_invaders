#include "Game.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ranges>

#include "Colors.h"
#include "MysteryShip.h"
#include "SpaceShip.h"

namespace SpaceInvaders {

Game::Game() {
    InitWindow(ScreenWidth + ScreenPadding, ScreenHeight + 2 * ScreenPadding, "Raylib Space Invaders!");

    GameResources->LoadTextures("Graphics");
    GameResources->LoadSounds("Sounds/Effects");
    GameResources->LoadMusic("Sounds/Music");
    GameResources->LoadFonts("Fonts");

    InitAudioDevice();
    LoadHighScore();
}

Game::~Game() {
    SaveHighScore();

    if (IsFontValid(m_font))
        UnloadFont(m_font);
    if (IsMusicValid(m_music))
        UnloadMusicStream(m_music);

    CloseAudioDevice();
    GameResources.reset(); // Resources need to be unloaded before CloseWindow() is called
    m_player.reset();
    m_mystery.reset();
    CloseWindow();
}

void
Game::Run() {
    const auto f = GameResources->GetFont("monogram.ttf");
    assert(f.has_value());
    m_font = f.value();

    const auto music = GameResources->GetMusic("music.ogg");
    assert(music.has_value());
    m_music = music.value();

    CreateAliens();
    CreateBarriers();

    m_player = std::make_unique<SpaceShip>();
    m_mystery = std::make_unique<MysteryShip>();

    PlayMusicStream(m_music);

    while (!WindowShouldClose()) {
        UpdateMusicStream(m_music);
        HandleInput();
        BeginDrawing();
        ClearBackground(Colors::Gray);
        DrawUI();

        if (!m_paused) {
            Update();
            CheckCollisions();
        }

        Draw();
        EndDrawing();
    }
}

void
Game::Update() {
    // Update Player position
    m_player->Update();
    m_mystery->Update();

    MoveAliens();

    // Pick a random alien to fire a laser
    const int16_t a_idx = GetRandomValue(0, AlienRows * AlienCols - 1);
    m_aliens[a_idx]->FireLaser();

    // Check for expired explosions
    std::ranges::remove_if(m_explosions, [](const auto &explosion) { return explosion->IsExpired(); });

    // Check for dead aliens
    std::ranges::for_each(m_aliens, [](auto &alien) { return !alien->Active(); });
}

void
Game::CheckCollisions() {
    using namespace std::ranges;

    // Player lasers can collide with aliens, barriers, alien lasers, and mystery ship
    for_each(m_player->GetLasers(), [this](auto &laser) {
        if (const auto entity = laser.CollidesWithAny(m_aliens); entity) {
            const auto alien = std::dynamic_pointer_cast<Alien>(entity);
            laser.Explode(true);
            alien->Explode();
        }

        if (const auto entity = laser.CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = laser.CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                barrier->Damage(laser.GetPosition());
                laser.Explode(true);
            }
        }

        if (laser.CollidesWith(*m_mystery)) { m_mystery->Explode(); }
    });
}

void
Game::Draw() {
    m_player->Draw();
    m_mystery->Draw();

    // Draw Barriers
    std::ranges::for_each(m_barriers, [](auto &barrier) { barrier->Draw(); });

    // Draw Aliens
    std::ranges::for_each(m_aliens, [](auto &alien) { alien->Draw(); });

    // Draw Explosions
    std::ranges::for_each(m_explosions, [](auto &explosion) { explosion->Draw(); });
}

void
Game::HandleInput() {
    if (IsKeyDown(KEY_P)) {
        m_paused = !m_paused;
    }
}

void
Game::CreateBarriers() {
    constexpr int16_t barrierWidth = Barrier::BarrierWidth;
    const float gap = (GetScreenWidth() - (4 * barrierWidth)) / 5;

    for (int8_t i = 0; i < 4; i++) {
        const float offX = (i + 1) * gap + i * barrierWidth;
        m_barriers[i] = std::make_unique<Barrier>(Vector2 { offX, GetScreenHeight() - 200.0f });
    }
}

void
Game::CreateAliens() {
    // First, create all aliens to determine their types and textures
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

    constexpr float horizontalSpacing = 10.0f; // Gap between alien columns
    constexpr float verticalSpacing = 10.0f;   // Gap between alien rows

    const float totalGridWidth = (AlienCols * maxAlienWidth) + ((AlienCols - 1) * horizontalSpacing);

    // Center the grid on screen
    const float startX = (GetScreenWidth() - totalGridWidth) / 2.0f;
    const float startY = 110.0f; // Start 110 pixels from top

    // Position each alien in its grid slot (centered within the max width)
    for (size_t i = 0; i < m_aliens.size(); i++) {
        const auto row = i / AlienCols;
        const auto col = i % AlienCols;

        const float slotX = startX + col * (maxAlienWidth + horizontalSpacing);
        const float slotY = startY + row * (maxAlienHeight + verticalSpacing);

        // Center the alien within its allocated slot
        const Texture2D &tex = m_aliens[i]->GetTexture();
        const float centeredX = slotX + (maxAlienWidth - tex.width) / 2.0f;
        const float centeredY = slotY + (maxAlienHeight - tex.height) / 2.0f;

        m_aliens[i]->Move({ centeredX, centeredY });
    }
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
Game::DrawUI() {
    DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, Colors::Yellow);
    DrawLineEx({25, 730}, {775, 730}, 3, Colors::Yellow);

    if (m_gameOver) {
        DrawTextEx(m_font, "GAME OVER", { 570, 740 }, 34, 2, Colors::Yellow);
    } else {
        DrawTextEx(m_font, "LEVEL 01", { 570, 740 }, 34, 2, Colors::Yellow);
    }

    for (uint8_t i = 0; i < m_playerLives; i++) {
        DrawTextureV(m_player->GetTexture(), {m_player->GetTexture().width + 50.0f * i, 745}, WHITE);
    }

    DrawTextEx(m_font, "SCORE", {50, 15}, 34, 2, Colors::Yellow);
    const auto scoreText = std::format("{:05d}", m_score);
    DrawTextEx(m_font, scoreText.c_str(), {50, 40}, 34, 2, Colors::Yellow);

    DrawTextEx(m_font, "HIGH-SCORE", {570, 15}, 34, 2, Colors::Yellow);
    const auto highScoreText = std::format("{:05d}", m_highScore);
    DrawTextEx(m_font, highScoreText.c_str(), {660, 40}, 34, 2, Colors::Yellow);
}

void
Game::AddExplosion(std::unique_ptr<Explosion> &explosion) {
    m_explosions.push_back(std::move(explosion));
}

void
Game::MoveAliens() const {
    bool moveDown = false;
    float maxAlienHeight = 0;
    for (const auto &alien : m_aliens) {
        alien->Update();
        // Check if the alien is off the screen
        if (alien->GetPosition().x < ScreenPadding / 2.0f ||
            alien->GetPosition().x + alien->GetTexture().width > GetScreenWidth() - ScreenPadding / 2) {
            moveDown = true;
        }

        maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(alien->GetTexture().height));
    }

    if (!moveDown) return;

    // Move aliens down if any alien was off the screen in the above block
    maxAlienHeight += 10.0f;   // Gap between alien rows
    for (const auto &alien : m_aliens) {
        alien->SetSpeed(-alien->GetSpeed());
        alien->Move({ alien->GetPosition().x + alien->GetSpeed(), alien->GetPosition().y + maxAlienHeight / 2 });
    }

    Alien::StepUpSpeed();
}

}
