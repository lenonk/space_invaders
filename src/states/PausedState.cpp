#include <format>

#include "Game.h"
#include "states/PausedState.h"
#include "states/MenuState.h"
#include "Colors.h"

namespace SpaceInvaders {

void PausedState::Enter(Game *game) {
    game->PauseMusicStream();
}

void PausedState::Exit(Game *game) {
    game->PlayMusicStream();
}

void PausedState::Update(Game *game) { }

void PausedState::Draw(Game *game) {
    // Draw the game behind the pause overlay
    game->Draw();
    game->DrawUI();
    
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, ColorAlpha(Colors::Black, 0.65f));
    
    const auto font = game->GetFont();
    const auto pauseText = "PAUSED";
    auto [px, py] = MeasureTextEx(font, pauseText, 64, 2);
    DrawTextEx(font, pauseText, 
              {Game::ScreenWidth / 2 - px / 2, Game::ScreenHeight / 2 - py / 2},
              64, 2, Colors::Yellow);

    const auto instruction = "PRESS P OR ESC TO RESUME";
    auto [rx, ry] = MeasureTextEx(font, instruction, 24, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - rx / 2, Game::ScreenHeight / 2 + 50},
              24, 2, WHITE);
}

void PausedState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->PopState(game);
    }
    else if (IsKeyPressed(KEY_M)) {
        // Return to menu
        Game::StateManager->ChangeState(std::make_unique<MenuState>(), game);
    }
}

}