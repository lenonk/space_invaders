#pragma once

#include <stack>
#include <memory>
#include "GameState.h"

namespace SpaceInvaders {

class Game;

class GameStateManager final {
public:
    GameStateManager() = default;
    ~GameStateManager() = default;

    void PushState(std::unique_ptr<GameState> state, Game *game);
    void PopState(Game *game);
    void ChangeState(std::unique_ptr<GameState> state, Game *game);
    void Update(Game *game);
    void Draw(Game *game);
    void HandleInput(Game *game);

    [[nodiscard]] bool IsEmpty() const { return m_states.empty(); }
    [[nodiscard]] GameState *GetCurrentState() const;

private:
    std::stack<std::unique_ptr<GameState>> m_states;
};

}
