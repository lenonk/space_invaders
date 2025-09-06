#pragma once
#include <vector>

#include "Entity.h"
#include "Laser.h"

namespace SpaceInvaders {

class SpaceShip final : public Entity {
public:
    const float Speed = 380.0f;
    const float FireSpeed = 0.2f;
    const double RespawnTime = 1.5f;
    const double InvulnerableTime = 2.0f;
    const uint8_t MaxLasers = 1;

    SpaceShip();
    ~SpaceShip() override = default;

    void Update() override;
    void Draw() override;
    void Reset();

    void FireLaser();
    void MoveLeft();
    void MoveRight();

    bool Die();

    [[nodiscard]] std::vector<Laser> &GetLasers() { return m_lasers; }

private:
    bool m_active {true};
    bool m_invulnerable {false};
    double m_respawnTimer {0};
    double m_invulnerableTimer {0};
    double m_lastFireTime {0};

    std::vector<Laser> m_lasers {};
};

}
