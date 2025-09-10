#pragma once
#include "GameState.h"
#include <raylib.h>

namespace SpaceInvaders {

// Menu State
class MenuState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;

private:
    enum class MenuOption { Play, HighScore, Quit };
    MenuOption m_selectedOption = MenuOption::Play;
    static constexpr int MenuOptionCount = 3;
};

// Playing State
class PlayingState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;
    void Pause(Game *game) override;
    void Resume(Game *game) override;
};

// Paused State
class PausedState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;
};

// Game Over State
class GameOverState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;

private:
    double m_stateEnterTime = 0.0;
    static constexpr double MinDisplayTime = 2.0; // Minimum time to show game over
};

// High Score State
class HighScoreState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;
};

}