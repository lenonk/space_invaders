#include <format>

#include "Game.h"
#include "states/QuitState.h"
#include "Colors.h"

namespace SpaceInvaders {

void QuitState::Enter(Game *game) {
}

void QuitState::Exit(Game *game) {
}

void QuitState::Update(Game *game) { }

void QuitState::Draw(Game *game) {
    // Draw the game behind the pause overlay
    game->Draw();
    game->DrawUI();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(Colors::Black, 0.65f));
    
    const auto font = game->GetFont();
    const auto quitText = "ARE YOU SURE YOU WANT TO QUIT?";
    auto [px, py] = MeasureTextEx(font, quitText, m_textMedium, 2);
    DrawTextEx(font, quitText,
              {GetScreenWidth() / 2 - px / 2, GetScreenHeight() / 2 - py / 2},
              m_textMedium, 2, Colors::Yellow);

    const auto instruction = "PRESS Y TO QUIT OR N TO CONTINUE";
    auto [rx, ry] = MeasureTextEx(font, instruction, m_textSmall, 2);
    DrawTextEx(font, instruction, 
              {GetScreenWidth() / 2 - rx / 2, GetScreenHeight() / 2.0f + 50.0f},
              m_textSmall, 2, WHITE);
}

void QuitState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->PopState(game);
    }
    else if (IsKeyPressed(KEY_Y)) {
        game->SetShouldExit(true);
    }
}

}