#pragma once
#include <memory>

#include "Alien.h"
#include "SpaceShip.h"
#include "Barrier.h"

namespace SpaceInvaders {
class MysteryShip;
constexpr int32_t ScreenWidth = 750;
constexpr int32_t ScreenHeight = 700;
constexpr int32_t TargetFPS = 60;

constexpr uint8_t AlienRows = 5;
constexpr uint8_t AlienCols = 11;

constexpr uint8_t NumBarriers = 4;

class Game final {
public:
    Game();
    ~Game();

    void Run();
    void Draw();

    void MoveAliens();

    void Update();
    void HandleInput() const;

    void CreateBarriers();
    void CreateAliens();

private:
    std::unique_ptr<SpaceShip> m_player {};
    std::unique_ptr<MysteryShip> m_mystery {};

    std::array<Barrier, NumBarriers> m_barriers;
    std::array<std::array<Alien, AlienCols>, AlienRows> m_aliens {};
};

}
