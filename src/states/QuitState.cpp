#include <format>

#include "Game.h"
#include "states/QuitState.h"
#include "Colors.h"

namespace SpaceInvaders {

void QuitState::Enter(Game *game) {
    game->PauseMusicStream();
}

void QuitState::Exit(Game *game) {
    game->PlayMusicStream();
}

void QuitState::Update(Game *game) { }

void QuitState::Draw(Game *game) {
    // Draw the game behind the pause overlay
    game->Draw();
    game->DrawUI();
    
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, ColorAlpha(Colors::Black, 0.65f));
    
    const auto font = game->GetFont();
    const auto quitText = "ARE YOU SURE YOU WANT TO QUIT?";
    auto [px, py] = MeasureTextEx(font, quitText, m_textMedium, 2);
    DrawTextEx(font, quitText,
              {Game::ScreenWidth / 2 - px / 2, Game::ScreenHeight / 2 - py / 2},
              m_textMedium, 2, Colors::Yellow);

    const auto instruction = "PRESS Y TO QUIT OR N TO CONTINUE";
    auto [rx, ry] = MeasureTextEx(font, instruction, m_textSmall, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - rx / 2, Game::ScreenHeight / 2 + 50},
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