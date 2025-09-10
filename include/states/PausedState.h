#pragma once
#include "GameState.h"

namespace SpaceInvaders {

class PausedState final : public GameState {
public:
    void Enter(Game *game) override;
    void Exit(Game *game) override;
    void Update(Game *game) override;
    void Draw(Game *game) override;
    void HandleInput(Game *game) override;
};

}