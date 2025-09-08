#include "Game.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>

#include "Colors.h"
#include "Logger.h"
#include "MysteryShip.h"
#include "SpaceShip.h"

namespace SpaceInvaders {

Game::Game() {
    InitWindow(ScreenWidth, ScreenHeight, "Raylib Space Invaders!");

    InitAudioDevice();

    GameResources->LoadTextures("Graphics");
    GameResources->LoadSounds("Sounds/Effects");
    GameResources->LoadMusic("Sounds/Music");
    GameResources->LoadFonts("Fonts");

    LoadHighScore();

    SetRandomSeed((int32_t)GetTime());
}

Game::~Game() {
    SaveHighScore();
    GameResources.reset(); // Resources need to be unloaded before CloseWindow() is called
    CloseAudioDevice();
    m_player.reset();
    m_mystery.reset();
    m_alienLasers.clear();
    m_explosions.clear();
    std::ranges::for_each(m_barriers, [](auto &barrier) { barrier.reset(); });
    std::ranges::for_each(m_aliens, [](auto &alien) { alien.reset(); });
    CloseWindow();
}

void
Game::Run() {
    const auto f = GameResources->GetFont("monogram.ttf");
    if (!f.has_value()) {
        LogError("Unable to load font: monogram.ttf");
        return;
    }

    const auto music = GameResources->GetMusic("music.ogg");
    if (!music.has_value()) {
        LogError("Unable to load music: music.ogg");
        return;
    }

    m_music = music.value();

    try {
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
    } catch (const std::runtime_error &e) {
        LogError(e.what());
    }
}

uint8_t
Game::AliensLeft() const {
    uint8_t count = 0;
    std::ranges::for_each(m_aliens, [&count](auto &alien) {if (alien->GetActive()) { ++count; }});
    return count;
}

void
Game::Update() const {
    // Update Mystery Ship
    m_mystery->Update();

    std::ranges::for_each(m_alienLasers, [](auto &laser) { laser->Update(); });
    std::erase_if(m_alienLasers, [](auto& laser) { return !laser->GetActive(); });

    // Check for expired explosions
    std::erase_if(m_explosions, [](const auto &explosion) { return explosion.IsExpired(); });

    // ***** Everything below here only happens if the game is not over.
    if (m_gameOver) { return; }

    // Update Player stuff
    m_player->Update();

    // Update aliens
    MoveAliens();

    // Pick a random alive alien to fire a laser (O(N) time, O(1) space, uniform)
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
Game::CheckPlayerCollisions() {
    using namespace std::ranges;

    if (!m_player) { return; }

    // Player lasers can collide with aliens, barriers, alien lasers, and mystery ship
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
            aLaser->Explode(false);
            IncrementScore(1000);
        }

        if (laser.CollidesWith(*m_mystery)) {
            laser.Explode(false);
            m_mystery->Explode();
            IncrementScore(500);
        }
    }
}

void
Game::CheckAlienCollisions() {
    using namespace std::ranges;

    // Alien lasers can collide with the player and barriers (and player lasers, but that doesn't need to be
    // handled here since it's already handled in CheckPlayerCollisions()
    for (const auto &laser : m_alienLasers) {
        if (m_player && laser->CollidesWith(*m_player)) {
            laser->Explode(false);
            if (m_player->Die()) {
                DecrementPlayerLives();
            }
        }

        // This may look confusing at first, but the idea is to check if the laser collides with the bounding
        // rect of the entire barrier before checking if it collides with any individual barrier cell.  This
        // Should be far more efficient since each barrier is 2691 pixels
        if (const auto entity = laser->CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = laser->CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                barrier->Damage(*laser);
                laser->Explode(true);
            }
        }
    }

    for (const auto &alien : m_aliens) {
        // Check collision with Barrier
        if (const auto entity = alien->CollidesWithAny(m_barriers); entity) {
            const auto barrier = std::dynamic_pointer_cast<Barrier>(entity);
            if (const auto cellEntity = alien->CollidesWithAny(barrier->GetCellRects()); cellEntity) {
                const auto cell = std::dynamic_pointer_cast<CellRect>(cellEntity);
                barrier->Damage(cell->GetPosition());
            }
        }

        // Finally, the alien itself can collide with the player
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
Game::IncrementScore(int16_t score) {
    m_score += score;
    if (m_score > m_highScore) {
        m_highScore = m_score;
    }
}

void
Game::Reset() {
    m_gameOver = false;
    m_score = 0;
    m_alienLasers.clear();
    m_explosions.clear();

    std::ranges::for_each(m_barriers, [](auto &barrier) { barrier.reset(); });
    std::ranges::for_each(m_aliens, [](auto &alien) { alien.reset(); });

    try {
        m_player = std::make_unique<SpaceShip>();
        m_mystery = std::make_unique<MysteryShip>();

        m_playerLives = 3;

        CreateAliens();
        CreateBarriers();
    } catch (const std::runtime_error &e) {
        LogError(e.what());
        std::terminate();
    }
}

void
Game::CheckCollisions() {
    if (m_paused) { return; }
    CheckPlayerCollisions();
    CheckAlienCollisions();
}

void
Game::Draw() {
    if (m_player)
        m_player->Draw();

    if (m_mystery)
        m_mystery->Draw();

    // Draw all the things...
    std::ranges::for_each(m_barriers, [](auto &barrier) { barrier->Draw(); });
    std::ranges::for_each(m_aliens, [](auto &alien) { alien->Draw(); });
    std::ranges::for_each(m_explosions, [](auto &explosion) { explosion.Draw(); });
    std::ranges::for_each(m_alienLasers, [](auto &laser) { laser->Draw(); });
}

void
Game::HandleInput() {
    if (m_gameOver && IsKeyPressed(KEY_SPACE)) {
        Reset();
    } else if (IsKeyPressed(KEY_P)) {
        m_paused = !m_paused;
        if (m_paused) {
            PauseMusicStream(m_music);
        } else {
            PlayMusicStream(m_music);
        }
    }
}

void
Game::CreateBarriers() {
    constexpr int16_t barrierWidth = Barrier::BarrierWidth;
    const float gap = (GetScreenWidth() - (4 * barrierWidth)) / 5;

    for (int8_t i = 0; i < 4; i++) {
        const float offX = (i + 1) * gap + i * barrierWidth;
        m_barriers[i] = std::make_unique<Barrier>(Vector2 { offX, GroundLevel - 100.0f });
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

    // Magic numbers...yeah yeah...I know
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
    // 10 is a magic number here, and I don't care.  It's just for positioning the frame around the view portal
    DrawRectangleRoundedLinesEx( {10, 10, ScreenHeight - 20, ScreenWidth -20}, 0.18f, 20, 2, Colors::Yellow);
    DrawLineEx( {ScreenPadding / 2, GroundLevel}, {ScreenWidth - ScreenPadding / 2, GroundLevel}, 3, Colors::Yellow);

    if (m_gameOver) {
        DrawTextEx(m_font, "GAME OVER", { 570, 740 }, 34, 2, Colors::Yellow);
        const auto text = "PRESS SPACE TO PLAY AGAIN";
        const auto size = MeasureTextEx(m_font, text, 34, 2);
        DrawTextEx(m_font, "PRESS SPACE TO PLAY AGAIN", { ScreenWidth / 2 - size.x / 2, ScreenHeight / 2 - size.y / 2 }, 34, 2, Colors::Yellow);
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
Game::AddExplosion(const Explosion &explosion) {
    m_explosions.push_back(explosion);
}

void
Game::AddAlienLaser(const std::shared_ptr<Laser>& laser) {
    m_alienLasers.push_back(laser);
}

void
Game::MoveAliens() const {
    bool moveDown = false;
    float maxAlienHeight = 0;
    for (const auto &alien : m_aliens) {
        alien->Update();
        // Check if the alien is off the screen
        if (alien->GetActive() && (alien->GetPosition().x < ScreenPadding / 2.0f ||
            alien->GetPosition().x + alien->GetTexture().width > GetScreenWidth() - ScreenPadding / 2)) {
            moveDown = true;
        }

        maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(alien->GetTexture().height));
    }

    const auto aliensLeft = AliensLeft();
    static auto lastTrigger = aliensLeft;
    if (aliensLeft > 0 && (aliensLeft / lastTrigger) * 100 < 90) {
        Alien::StepUpSpeed();
        lastTrigger = aliensLeft;
    }

    if (!moveDown) return;

    // Move aliens down if any alien was off the screen in the above block
    maxAlienHeight += 10.0f;   // Gap between alien rows
    for (const auto &alien : m_aliens) {
        alien->SetSpeed(-alien->GetSpeed());
        alien->Move({ alien->GetPosition().x + alien->GetSpeed(), alien->GetPosition().y + maxAlienHeight / 2 });
    }
}

}
