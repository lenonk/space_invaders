#include <format>

#include "Game.h"
#include "Colors.h"
#include "states/GameOverState.h"
#include "states/MenuState.h"
#include "states/PlayingState.h"

namespace SpaceInvaders {

void GameOverState::Enter(Game *game) {
    m_stateEnterTime = GetTime();
    game->PauseMusicStream();
}

void GameOverState::Exit(Game *game) { }

void GameOverState::Update(Game *game) {
    game->UpdateVisualEffects();
}

void GameOverState::Draw(Game *game) {
    game->Draw();
    game->DrawUI();
    
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, ColorAlpha(Colors::Black, 0.65f));

    const auto font = game->GetFont();
    const auto gameOverText = "GAME OVER";
    auto textSize = MeasureTextEx(font, gameOverText, m_textLarge, 2);
    DrawTextEx(font, gameOverText, 
              {Game::ScreenWidth / 2 - textSize.x / 2, Game::ScreenHeight / 2 - 100},
              m_textLarge, 2, Colors::Yellow);
    
    const std::string scoreText = std::format("FINAL SCORE: {:05d}", game->GetScore());
    auto scoreSize = MeasureTextEx(font, scoreText.c_str(), m_textMedium, 2);
    DrawTextEx(font, scoreText.c_str(), 
              {Game::ScreenWidth / 2 - scoreSize.x / 2, Game::ScreenHeight / 2 - 20}, 
              m_textMedium, 2, WHITE);
    
    if (GetTime() - m_stateEnterTime > MinDisplayTime) {
        const auto instruction = "PRESS SPACE TO PLAY AGAIN OR ESC FOR MENU";
        auto instrSize = MeasureTextEx(font, instruction, m_textSmall, 2);
        DrawTextEx(font, instruction, 
                  {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight / 2 + 50}, 
                  m_textSmall, 2, GRAY);
    }
}

void GameOverState::HandleInput(Game *game) {
    if (GetTime() - m_stateEnterTime < MinDisplayTime) { return; }
    
    if (IsKeyPressed(KEY_SPACE)) {
        Game::StateManager->ChangeState(std::make_unique<PlayingState>(), game);
    }
    else if (IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->ChangeState(std::make_unique<MenuState>(), game);
    }
}

}
