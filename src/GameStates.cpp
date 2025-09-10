#include "GameStates.h"
#include "Game.h"
#include "Colors.h"

namespace SpaceInvaders {

// MenuState Implementation
void MenuState::Enter(Game* game) {
    game->PauseMusicStream();
}

void MenuState::Exit(Game* game) {
    // Nothing specific needed
}

void MenuState::Update(Game* game) {
    // Menu doesn't need complex updates
}

void MenuState::Draw(Game* game) {
    const Font& font = game->GetFont();
    
    // Draw title
    const char* title = "SPACE INVADERS";
    auto titleSize = MeasureTextEx(font, title, 64, 2);
    DrawTextEx(font, title, 
              {Game::ScreenWidth / 2 - titleSize.x / 2, 200}, 
              64, 2, Colors::Yellow);
    
    // Draw menu options
    const char* options[] = {"PLAY", "HIGH SCORES", "QUIT"};
    const float startY = 350.0f;
    const float spacing = 60.0f;
    
    for (int i = 0; i < MenuOptionCount; ++i) {
        Color color = (static_cast<int>(m_selectedOption) == i) ? Colors::Yellow : WHITE;
        auto optionSize = MeasureTextEx(font, options[i], 34, 2);
        DrawTextEx(font, options[i], 
                  {Game::ScreenWidth / 2 - optionSize.x / 2, startY + i * spacing}, 
                  34, 2, color);
    }
    
    // Instructions
    const char* instruction = "USE ARROW KEYS TO NAVIGATE, SPACE TO SELECT";
    auto instrSize = MeasureTextEx(font, instruction, 20, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight - 100}, 
              20, 2, GRAY);
}

void MenuState::HandleInput(Game* game) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedOption = static_cast<MenuOption>((static_cast<int>(m_selectedOption) - 1 + MenuOptionCount) % MenuOptionCount);
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedOption = static_cast<MenuOption>((static_cast<int>(m_selectedOption) + 1) % MenuOptionCount);
    }
    else if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (m_selectedOption) {
            case MenuOption::Play:
                game->GetStateManager().ChangeState(std::make_unique<PlayingState>(), game);
                break;
            case MenuOption::HighScore:
                game->GetStateManager().PushState(std::make_unique<HighScoreState>(), game);
                break;
            case MenuOption::Quit:
                // Set a flag for the main loop to exit
                game->SetShouldExit(true);
                break;
        }
    }
}

// PlayingState Implementation
void PlayingState::Enter(Game* game) {
    game->InitializeGameplay();
    game->PlayMusicStream();
}

void PlayingState::Exit(Game* game) {
    game->PauseMusicStream();
}

void PlayingState::Update(Game* game) {
    game->UpdateGameplay();
    game->CheckCollisions();
    
    // Check for game over condition
    if (game->IsGameOver()) {
        game->GetStateManager().ChangeState(std::make_unique<GameOverState>(), game);
    }
}

void PlayingState::Draw(Game* game) {
    game->DrawGameplay();
    game->DrawUI();
}

void PlayingState::HandleInput(Game* game) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        game->GetStateManager().PushState(std::make_unique<PausedState>(), game);
    }
    
    game->HandleGameplayInput();
}

void PlayingState::Pause(Game* game) {
    game->PauseMusicStream();
}

void PlayingState::Resume(Game* game) {
    game->PlayMusicStream();
}

// PausedState Implementation
void PausedState::Enter(Game* game) {
    game->PauseMusicStream();
}

void PausedState::Exit(Game* game) {
    game->PlayMusicStream();
}

void PausedState::Update(Game* game) {
    // No updates while paused
}

void PausedState::Draw(Game* game) {
    // Draw the game behind the pause overlay
    game->DrawGameplay();
    game->DrawUI();
    
    // Draw pause overlay
    DrawRectangle(0, 0, Game::ScreenWidth, Game::ScreenHeight, ColorAlpha(BLACK, 0.5f));
    
    const Font& font = game->GetFont();
    const char* pauseText = "PAUSED";
    auto textSize = MeasureTextEx(font, pauseText, 64, 2);
    DrawTextEx(font, pauseText, 
              {Game::ScreenWidth / 2 - textSize.x / 2, Game::ScreenHeight / 2 - textSize.y / 2}, 
              64, 2, Colors::Yellow);
    
    const char* instruction = "PRESS P OR ESC TO RESUME";
    auto instrSize = MeasureTextEx(font, instruction, 24, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight / 2 + 50}, 
              24, 2, WHITE);
}

void PausedState::HandleInput(Game* game) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        game->GetStateManager().PopState(game);
    }
    else if (IsKeyPressed(KEY_M)) {
        // Return to menu
        game->GetStateManager().ChangeState(std::make_unique<MenuState>(), game);
    }
}

// GameOverState Implementation
void GameOverState::Enter(Game* game) {
    m_stateEnterTime = GetTime();
    game->PauseMusicStream();
}

void GameOverState::Exit(Game* game) {
    // Nothing specific needed
}

void GameOverState::Update(Game* game) {
    // Continue updating explosions and other visual effects
    game->UpdateVisualEffects();
}

void GameOverState::Draw(Game* game) {
    game->DrawGameplay();
    game->DrawUI();
    
    const Font& font = game->GetFont();
    const char* gameOverText = "GAME OVER";
    auto textSize = MeasureTextEx(font, gameOverText, 64, 2);
    DrawTextEx(font, gameOverText, 
              {Game::ScreenWidth / 2 - textSize.x / 2, Game::ScreenHeight / 2 - 100}, 
              64, 2, Colors::Yellow);
    
    // Show final score
    const std::string scoreText = std::format("FINAL SCORE: {:05d}", game->GetScore());
    auto scoreSize = MeasureTextEx(font, scoreText.c_str(), 34, 2);
    DrawTextEx(font, scoreText.c_str(), 
              {Game::ScreenWidth / 2 - scoreSize.x / 2, Game::ScreenHeight / 2 - 20}, 
              34, 2, WHITE);
    
    if (GetTime() - m_stateEnterTime > MinDisplayTime) {
        const char* instruction = "PRESS SPACE TO PLAY AGAIN OR ESC FOR MENU";
        auto instrSize = MeasureTextEx(font, instruction, 24, 2);
        DrawTextEx(font, instruction, 
                  {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight / 2 + 50}, 
                  24, 2, GRAY);
    }
}

void GameOverState::HandleInput(Game* game) {
    if (GetTime() - m_stateEnterTime < MinDisplayTime) {
        return; // Don't allow input too quickly
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        game->GetStateManager().ChangeState(std::make_unique<PlayingState>(), game);
    }
    else if (IsKeyPressed(KEY_ESCAPE)) {
        game->GetStateManager().ChangeState(std::make_unique<MenuState>(), game);
    }
}

// HighScoreState Implementation
void HighScoreState::Enter(Game* game) {
    // Nothing specific needed
}

void HighScoreState::Exit(Game* game) {
    // Nothing specific needed
}

void HighScoreState::Update(Game* game) {
    // Static screen, no updates needed
}

void HighScoreState::Draw(Game* game) {
    const Font& font = game->GetFont();
    
    const char* title = "HIGH SCORES";
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
    
    const char* instruction = "PRESS ESC TO RETURN";
    auto instrSize = MeasureTextEx(font, instruction, 24, 2);
    DrawTextEx(font, instruction, 
              {Game::ScreenWidth / 2 - instrSize.x / 2, Game::ScreenHeight - 100}, 
              24, 2, GRAY);
}

void HighScoreState::HandleInput(Game* game) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->GetStateManager().PopState(game);
    }
}

}