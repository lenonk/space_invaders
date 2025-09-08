#pragma once
#include <memory>
#include <array>

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
    static constexpr int32_t ScreenWidth = 800;
    static constexpr int32_t ScreenHeight = 800;
    static constexpr float GroundLevel = ScreenHeight - ScreenPadding * 1.5;

    static inline auto GameResources = std::make_unique<ResourceManager>();

    Game();
    ~Game();

    void Run();

    void Draw() const;
    void DrawUI();

    void MoveAliens() const;
    void Update() const;

    void CheckCollisions();

    void CheckPlayerCollisions();
    void CheckAlienCollisions();
    void DecrementPlayerLives();
    void IncrementScore(int16_t score);
    void Reset();

    void HandleInput();
    void CreateBarriers();
    void CreateAliens();

    void SaveHighScore() const;
    void LoadHighScore();

    [[nodiscard]] uint8_t AliensLeft() const;

    static void AddExplosion(const Explosion &explosion);
    static void AddAlienLaser(const std::shared_ptr<AlienLaser>& laser);
    static bool GamePaused() { return m_paused; }

private: // Constants
    static constexpr uint8_t AlienRows      = 5;
    static constexpr uint8_t AlienCols      = 11;
    static constexpr uint8_t NumBarriers    = 4;

    const uint8_t FontSize      = 34;
    const uint8_t FontSpacing   = 2;

private:
    bool m_gameOver         {false};
    uint8_t m_playerLives   {3};
    uint32_t m_score        {0};
    uint32_t m_highScore    {0};
    Font m_font             {};
    Music m_music           {};

    inline static bool m_paused           {false};

    std::unique_ptr<SpaceShip> m_player      {};
    std::unique_ptr<MysteryShip> m_mystery   {};

    std::array<std::shared_ptr<Barrier>, NumBarriers> m_barriers        {};
    std::array<std::shared_ptr<Alien>, AlienRows * AlienCols> m_aliens  {};

    inline static std::vector<Explosion> m_explosions                       {};
    inline static std::vector<std::shared_ptr<AlienLaser>> m_alienLasers    {};
};

}
