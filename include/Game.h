#pragma once

#include <algorithm>
#include <memory>
#include <array>

#include "Alien.h"
#include "SpaceShip.h"
#include "MysteryShip.h"
#include "Barrier.h"
#include "BloomRenderer.h"
#include "ParticleSystem.h"
#include "ResourceManager.h"
#include "states/GameStateManager.h"

namespace SpaceInvaders {

class Game final {
public:
    static constexpr int32_t ScreenPadding = 50;
    static constexpr int32_t ScreenWidth = 1080;
    static constexpr int32_t ScreenHeight = 720;

    static inline auto Resources    = std::make_unique<ResourceManager>();
    static inline auto StateManager = std::make_unique<GameStateManager>();
    static inline auto ParticleManager = std::make_unique<ParticleSystem>();

    Game();
    ~Game();

    void Run();
    void Draw() const;
    void DrawUI();
    void MoveAliens() const;
    void Update();
    void UpdateVisualEffects() ;
    void Reset();
    void DecrementPlayerLives();
    void IncrementScore(int16_t score);
    void HandleInput() const;

    void CheckCollisions();
    void CheckPlayerCollisions();
    void CheckAlienCollisions();

    void CreateBarriers();
    void CreateAliens();

    void SaveHighScore() const;
    void LoadHighScore();

    void PlayMusicStream() const { ::PlayMusicStream(m_music); }
    void PauseMusicStream() const { ::PauseMusicStream(m_music); }

    void SetShouldExit(const bool shouldExit) { m_shouldExit = shouldExit; }

    [[nodiscard]] auto IsGameOver() const { return m_gameOver; }
    [[nodiscard]] auto GetAliensLeft() const { return std::ranges::count_if(m_aliens, [](const auto &alien) { return alien->GetActive(); }); }
    [[nodiscard]] auto GetScore() const { return m_score; }
    [[nodiscard]] auto GetHighScore() const { return m_highScore; }
    [[nodiscard]] auto &GetFont() const { return m_font; }

    static void AddAlienLaser(const std::shared_ptr<AlienLaser>& laser);
    static float GetGroundLevel() { return GetScreenHeight() - ScreenPadding * 1.5; }

private: // Constants
    static constexpr uint8_t AlienRows      = 5;
    static constexpr uint8_t AlienCols      = 11;
    static constexpr uint8_t NumBarriers    = 4;

    const uint8_t PlayerLives   = 3;
    const uint8_t FontSize      = 34;
    const uint8_t FontSpacing   = 2;

private:
    bool m_gameOver         {false};
    bool m_shouldExit       {false};
    uint8_t m_level         {1};
    uint8_t m_playerLives   {PlayerLives};
    uint32_t m_score        {0};
    uint32_t m_highScore    {0};
    Font m_font             {};
    Music m_music           {};

    BloomRenderer m_bloomRenderer {};

    mutable float m_alienMoveTrigger  {0};

    std::unique_ptr<SpaceShip> m_player      {};
    std::unique_ptr<MysteryShip> m_mystery   {};

    std::array<std::shared_ptr<Barrier>, NumBarriers> m_barriers        {};
    std::array<std::shared_ptr<Alien>, AlienRows * AlienCols> m_aliens  {};

    inline static std::vector<std::shared_ptr<AlienLaser>> m_alienLasers {};
};

}
