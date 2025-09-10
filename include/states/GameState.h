#pragma once

namespace SpaceInvaders {

class Game; // Forward declaration

class GameState {
public:
    GameState() = default;
    virtual ~GameState() = default;

    virtual void Enter(Game* game) = 0;
    virtual void Exit(Game* game) = 0;
    virtual void Update(Game* game) = 0;
    virtual void Draw(Game* game) = 0;
    virtual void HandleInput(Game* game) = 0;

    // Optional callbacks
    virtual void Pause(Game* game) {}
    virtual void Resume(Game* game) {}

protected:
    uint8_t m_textLarge     {64};
    uint8_t m_textMedium    {34};
    uint8_t m_textSmall     {24};
};

}