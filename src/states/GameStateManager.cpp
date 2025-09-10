#include "../../include/states/GameStateManager.h"
#include "Game.h"

namespace SpaceInvaders {

void GameStateManager::PushState(std::unique_ptr<GameState> state, Game *game) {
    if (!m_states.empty()) {
        m_states.top()->Pause(game);
    }

    m_states.push(std::move(state));
    m_states.top()->Enter(game);
}

void GameStateManager::PopState(Game *game) {
    if (!m_states.empty()) {
        m_states.top()->Exit(game);
        m_states.pop();

        if (!m_states.empty()) {
            m_states.top()->Resume(game);
        }
    }
}

void GameStateManager::ChangeState(std::unique_ptr<GameState> state, Game *game) {
    if (!m_states.empty()) {
        m_states.top()->Exit(game);
        m_states.pop();
    }

    m_states.push(std::move(state));
    m_states.top()->Enter(game);
}

void GameStateManager::Update(Game *game) {
    if (!m_states.empty()) {
        m_states.top()->Update(game);
    }
}

void GameStateManager::Draw(Game *game) {
    if (!m_states.empty()) {
        m_states.top()->Draw(game);
    }
}

void GameStateManager::HandleInput(Game *game) {
    if (!m_states.empty()) {
        m_states.top()->HandleInput(game);
    }
}

GameState* GameStateManager::GetCurrentState() const {
    return m_states.empty() ? nullptr : m_states.top().get();
}

}