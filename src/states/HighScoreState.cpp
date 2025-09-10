#include <format>

#include "Game.h"
#include "states/HighScoreState.h"
#include "Colors.h"

namespace SpaceInvaders {

void HighScoreState::Enter(Game *game) { }

void HighScoreState::Exit(Game *game) { }

void HighScoreState::Update(Game *game) { }

void HighScoreState::Draw(Game *game) {
    const auto font = game->GetFont();

    const auto title = "HIGH SCORES";
    auto titleSize = MeasureTextEx(font, title, 48, 2);
    DrawTextEx(font, title, 
              {Game::ScreenWidth / 2 - titleSize.x / 2, 150}, 
              48, 2, Colors::Yellow);
    
    // Display high score
    const std::string highScoreText = std::format("HIGH SCORE: {:05d}", game->GetHighScore());
    auto scoreSize = MeasureTextEx(font, highScoreText.c_str(), 34, 2);
    DrawTextEx(font, highScoreText.c_str(), 
              {Game::ScreenWidth / 2 - scoreSize.x / 2, 300}, 
              34, 2, WHITE);

    const auto instruction = "PRESS ESC TO RETURN";
    auto instrSize = MeasureTextEx(font, instruction, 24, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight - 100}, 
              24, 2, GRAY);
}

void HighScoreState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->PopState(game);
    }
}

}