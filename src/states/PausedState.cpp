#include <format>

#include "Game.h"
#include "states/PausedState.h"
#include "states/MenuState.h"
#include "Colors.h"

namespace SpaceInvaders {

void PausedState::Enter(Game *game) {
}

void PausedState::Exit(Game *game) {
}

void PausedState::Update(Game *game) { }

void PausedState::Draw(Game *game) {
    // Draw the game behind the pause overlay
    game->Draw();
    game->DrawUI();

    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, ColorAlpha(Colors::Black, 0.65f));
    
    const auto font = game->GetFont();
    const auto pauseText = "PAUSED";
    auto [px, py] = MeasureTextEx(font, pauseText, m_textLarge, 2);
    DrawTextEx(font, pauseText, 
              {GetScreenWidth() / 2 - px / 2, GetScreenHeight() / 2 - py / 2},
              m_textLarge, 2, Colors::Yellow);

    const auto instruction = "PRESS P OR ESC TO RESUME OR Q TO QUIT";
    auto [rx, ry] = MeasureTextEx(font, instruction, m_textSmall, 2);
    DrawTextEx(font, instruction, 
              {GetScreenWidth() / 2 - rx / 2, GetScreenHeight() / 2.0f + 50.0f},
              m_textSmall, 2, WHITE);
}

void PausedState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->PopState(game);
    }
    else if (IsKeyPressed(KEY_M)) {
        Game::StateManager->ChangeState(std::make_unique<MenuState>(), game);
    }
    else if (IsKeyPressed(KEY_Q)) {
        game->SetShouldExit(true);
    }
}

}