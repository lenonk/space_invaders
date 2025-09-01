#pragma once
#include <memory>

#include "Alien.h"
#include "SpaceShip.h"
#include "MysteryShip.h"
#include "Barrier.h"
#include "Explosion.h"
#include "ResourceManager.h"

namespace SpaceInvaders {

class Game final {
public:
    static constexpr int32_t ScreenPadding = 50;
    static constexpr int32_t ScreenWidth = 750;
    static constexpr int32_t ScreenHeight = 700;
    static constexpr int32_t TargetFPS = 60;

    static constexpr uint8_t AlienRows = 5;
    static constexpr uint8_t AlienCols = 11;

    static constexpr uint8_t NumBarriers = 4;
    static inline auto GameResources = std::make_unique<ResourceManager>();

    Game();
    ~Game();

    void Run();
    void Draw();
    void DrawUI();

    void MoveAliens() const;
    void Update();
    void CheckCollisions();
    void HandleInput();
    void CreateBarriers();
    void CreateAliens();

    void SaveHighScore() const;
    void LoadHighScore();

    static void AddExplosion(std::unique_ptr<Explosion> &explosion);

private:
    bool m_gameOver         {false};
    bool m_paused           {false};
    uint8_t m_playerLives   {3};
    uint32_t m_score        {0};
    uint32_t m_highScore    {0};
    uint32_t m_playerScore  {0};
    Font m_font             {};
    Music m_music           {};

    std::unique_ptr<SpaceShip> m_player      {};
    std::unique_ptr<MysteryShip> m_mystery   {};

    std::array<std::shared_ptr<Barrier>, NumBarriers> m_barriers;
    std::array<std::shared_ptr<Alien>, AlienRows * AlienCols> m_aliens {};

    inline static std::vector<std::unique_ptr<Explosion>> m_explosions;
};

}
