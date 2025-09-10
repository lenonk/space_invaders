#include <format>

#include "Game.h"
#include "states/MenuState.h"
#include "states/HighScoreState.h"
#include "states/PlayingState.h"
#include "Colors.h"

namespace SpaceInvaders {

void MenuState::Enter(Game *game) {
    game->PauseMusicStream();
}

void MenuState::Exit(Game *game) { }

void MenuState::Update(Game *game) { }

void MenuState::Draw(Game *game) {
    const Font &font = game->GetFont();
    
    // Draw title
    const auto title = "SPACE INVADERS";
    auto [mx, my] = MeasureTextEx(font, title, m_textLarge, 2);
    DrawTextEx(font, title, 
              {Game::ScreenWidth / 2 - mx / 2, 200},
              m_textLarge, 2, Colors::Yellow);
    
    // Draw menu options
    for (int i = 0; i < MenuOptionCount; ++i) {
        constexpr float spacing = 60.0f;
        constexpr float startY = 350.0f;
        const char *options[] = {"PLAY", "HIGH SCORES", "QUIT"};
        const auto color = (static_cast<int>(m_selectedOption) == i) ? Colors::Yellow : WHITE;
        auto [tx, ty] = MeasureTextEx(font, options[i], m_textMedium, 2);
        DrawTextEx(font, options[i], 
                  {Game::ScreenWidth / 2 - tx / 2, startY + i * spacing},
                  m_textMedium, 2, color);
    }
    
    // Instructions
    const auto instruction = "USE ARROW KEYS TO NAVIGATE, SPACE TO SELECT";
    auto [tx, ty] = MeasureTextEx(font, instruction, m_textSmall, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - tx / 2, Game::ScreenHeight - 100},
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
