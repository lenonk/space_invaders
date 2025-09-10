#include "Game.h"
#include "states/PlayingState.h"
#include "states/GameOverState.h"
#include "states/PausedState.h"
#include "states/QuitState.h"
#include "Colors.h"

namespace SpaceInvaders {

void PlayingState::Enter(Game *game) {
    game->Reset();
    game->PlayMusicStream();
}

void PlayingState::Exit(Game *game) {
    game->PauseMusicStream();
}

void PlayingState::Update(Game *game) {
    game->Update();
    game->CheckCollisions();
    
    // Check for game over condition
    if (game->IsGameOver()) {
        Game::StateManager->ChangeState(std::make_unique<GameOverState>(), game);
    }
}

void PlayingState::Draw(Game *game) {
    game->Draw();
    game->DrawUI();
}

void PlayingState::HandleInput(Game *game) {
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
        Game::StateManager->PushState(std::make_unique<PausedState>(), game);
    }
    if (IsKeyDown(KEY_Q)) {
        Game::StateManager->PushState(std::make_unique<QuitState>(), game);
    }
    game->HandleInput();
}

void PlayingState::Pause(Game *game) {
    game->PauseMusicStream();
}

void PlayingState::Resume(Game *game) {
    game->PlayMusicStream();
}

}
