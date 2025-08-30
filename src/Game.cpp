#include "Game.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "Colors.h"
#include "MysteryShip.h"
#include "SpaceShip.h"

namespace SpaceInvaders {

Game::Game() {
    InitWindow(ScreenWidth, ScreenHeight, "Raylib Space Invaders!");
    m_player = std::make_unique<SpaceShip>();
    m_mystery = std::make_unique<MysteryShip>();

    CreateBarriers();
    CreateAliens();
}

Game::~Game() {
    Alien::UnloadTextures();
    Laser::UnloadTexture();
    m_mystery.reset();
    m_player.reset();
    CloseWindow();
}

void
Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        BeginDrawing();
        ClearBackground(Colors::Gray);
        Update();
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
    int16_t a_idx = GetRandomValue(0, AlienRows * AlienCols - 1);
    int8_t row = a_idx / AlienCols;
    int8_t col = a_idx % AlienCols;
    m_aliens[row][col].FireLaser();
}

void
Game::Draw() {
    m_player->Draw();
    m_mystery->Draw();

    // Draw Barriers
    std::ranges::for_each(m_barriers, [](auto &barrier) { barrier.Draw(); });

    // Draw Aliens
    std::ranges::for_each(m_aliens, [](auto &aliens) {
        std::ranges::for_each(aliens, [](auto &alien) { alien.Draw(); });
    });
}

void
Game::HandleInput() const {
    if (IsKeyDown(KEY_LEFT)) {
        m_player->MoveLeft();
    }
    if (IsKeyDown(KEY_RIGHT)) {
        m_player->MoveRight();
    }
    if (IsKeyDown(KEY_SPACE)) {
        m_player->FireLaser();
    }
}

void
Game::CreateBarriers() {
    int16_t barrierWidth = Barrier::BarrierWidth * Barrier::CellWidth;
    float gap = (ScreenWidth - (4 * barrierWidth)) / 5;

    for (int8_t i = 0; i < 4; i++) {
        const float offX = (i + 1) * gap + i * barrierWidth;
        m_barriers[i] = Barrier({ offX, ScreenHeight - 100 });
    }
}

void
Game::CreateAliens() {
    // First, create all aliens to determine their types and textures
    for (uint8_t row = 0; row < AlienRows; row++) {
        for (uint8_t col = 0; col < AlienCols; col++) {
            uint8_t type = 3;
            if (row > 2) { type = 1; }
            else if (row > 0) { type = 2; }
            m_aliens[row][col] = Alien({ 0, 0 }, type);
        }
    }

    // Find the maximum alien width across all types
    float maxAlienWidth = 0.0f;
    float maxAlienHeight = 0.0f;

    for (uint8_t row = 0; row < AlienRows; row++) {
        for (uint8_t col = 0; col < AlienCols; col++) {
            Texture2D& tex = m_aliens[row][col].Texture();
            maxAlienWidth = std::max(maxAlienWidth, static_cast<float>(tex.width));
            maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(tex.height));
        }
    }

    constexpr float horizontalSpacing = 10.0f; // Gap between alien columns
    constexpr float verticalSpacing = 10.0f;   // Gap between alien rows

    const float totalGridWidth = (AlienCols * maxAlienWidth) + ((AlienCols - 1) * horizontalSpacing);

    // Center the grid on screen
    const float startX = (ScreenWidth - totalGridWidth) / 2.0f;
    const float startY = 110.0f; // Start 110 pixels from top

    // Position each alien in its grid slot (centered within the max width)
    for (uint8_t row = 0; row < AlienRows; row++) {
        for (uint8_t col = 0; col < AlienCols; col++) {
            const float slotX = startX + col * (maxAlienWidth + horizontalSpacing);
            const float slotY = startY + row * (maxAlienHeight + verticalSpacing);

            // Center the alien within its allocated slot
            Texture2D& tex = m_aliens[row][col].Texture();
            const float centeredX = slotX + (maxAlienWidth - tex.width) / 2.0f;
            const float centeredY = slotY + (maxAlienHeight - tex.height) / 2.0f;

            m_aliens[row][col].Move({ centeredX, centeredY });
        }
    }
}

void
Game::MoveAliens() {
    bool moveDown = false;
    float maxAlienHeight = 0;
    std::ranges::for_each(m_aliens, [&moveDown, &maxAlienHeight](auto &aliens) {
        std::ranges::for_each(aliens, [&moveDown, &maxAlienHeight](auto &alien) {
            alien.Update();
            // Check if the alien is off the screen
            maxAlienHeight = std::max(maxAlienHeight, static_cast<float>(alien.Texture().height));
            if (Alien::Direction() == Alien::Right && alien.Position().x + alien.Texture().width > ScreenWidth) {
                moveDown = true;
            }
            else if (Alien::Direction() == Alien::Left && alien.Position().x < 0) {
                moveDown = true;
            }
        });
    });

    // Move aliens down if any alien was off the screen in the above block
    maxAlienHeight += 10.0f;   // Gap between alien rows

    if (moveDown) {
        std::ranges::for_each(m_aliens, [&maxAlienHeight](auto &aliens) {
            std::ranges::for_each(aliens, [&maxAlienHeight](auto &alien) {
                if (Alien::Direction() == Alien::Right) {
                    alien.Move({ alien.Position().x - Alien::Speed, alien.Position().y + maxAlienHeight });
                }
                else {
                    alien.Move({ alien.Position().x + Alien::Speed, alien.Position().y + maxAlienHeight });
                }
            });
        });

        // Swap alien directions
        Alien::Direction(Alien::Direction() == Alien::Right ? Alien::Left : Alien::Right);
    }
}

}
