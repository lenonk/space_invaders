#include <format>

#include "Game.h"
#include "states/MenuState.h"

#include <cmath>

#include "states/HighScoreState.h"
#include "states/PlayingState.h"
#include "Colors.h"

namespace SpaceInvaders {

void MenuState::Enter(Game *game) {
}

void MenuState::Exit(Game *game) { }

void MenuState::Update(Game *game) { }

void MenuState::Draw(Game *game) {
    if (const auto title = Game::Resources->Get<Texture2D>("space_invaders_title.png"); title.has_value()) {
        const Texture2D &titleTexture = title.value();

        const Rectangle source = {0, 0, static_cast<float>(titleTexture.width), static_cast<float>(titleTexture.height)};
        const Rectangle dest = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};

        DrawTexturePro(titleTexture, source, dest, {0, 0}, 0.0f, WHITE);
    }

    const Font &font = game->GetFont();

    constexpr float fadeSpeed = 2.0f;
    constexpr float minAlpha = 100.0f;
    constexpr float maxAlpha = 255.0f;

    const float alpha = minAlpha + (maxAlpha - minAlpha) * (0.5f + 0.5f * sinf(GetTime() * fadeSpeed));

    for (int i = 0; i < MenuOptionCount; ++i) {
        constexpr float spacing = 55.0f;
        constexpr float startY = 480.0f;
        const char *options[] = {"PLAY", "HIGH SCORES", "QUIT"};

        Color color;
        if (static_cast<int>(m_selectedOption) == i) {
            color = {Colors::Yellow.r, Colors::Yellow.g, Colors::Yellow.b, static_cast<unsigned char>(alpha)};
        } else {
            color = WHITE;
        }

        auto [tx, ty] = MeasureTextEx(font, options[i], m_textMenu, 2);
        DrawTextEx(font, options[i],
                  {GetScreenWidth() / 2 - tx / 2, startY + i * spacing},
                  m_textMenu, 2, color);

    }
    
    // Instructions
    const auto instruction = "USE ARROW KEYS TO NAVIGATE, SPACE TO SELECT";
    auto [tx, ty] = MeasureTextEx(font, instruction, m_textSmall, 2);
    DrawTextEx(font, instruction, 
              {GetScreenWidth() / 2 - tx / 2, GetScreenHeight() - ty - 10.0f},
              m_textSmall, 2, GRAY);
}

void MenuState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedOption = static_cast<MenuOption>((static_cast<int>(m_selectedOption) - 1 + MenuOptionCount) % MenuOptionCount);
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedOption = static_cast<MenuOption>((static_cast<int>(m_selectedOption) + 1) % MenuOptionCount);
    }
    else if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (m_selectedOption) {
            case MenuOption::Play:
                Game::StateManager->ChangeState(std::make_unique<PlayingState>(), game);
                break;
            case MenuOption::HighScore:
                Game::StateManager->PushState(std::make_unique<HighScoreState>(), game);
                break;
            case MenuOption::Quit:
                game->SetShouldExit(true);
                break;
        }
    }
}

}
