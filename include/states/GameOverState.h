#pragma once
#include "GameState.h"

namespace SpaceInvaders {

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

}