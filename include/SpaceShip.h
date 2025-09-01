#pragma once
#include <vector>

#include "Entity.h"
#include "Laser.h"

namespace SpaceInvaders {

class SpaceShip final : public Entity {
public:
    static constexpr float Speed = 420.0f;
    static constexpr float FireSpeed = 0.2f;
    static constexpr uint8_t MaxLasers = 3;

    SpaceShip();
    ~SpaceShip() override = default;

    void Update() override;
    void Draw() override;

    void FireLaser();
    void MoveLeft();
    void MoveRight();

    void Explode();

    [[nodiscard]] std::vector<Laser> &GetLasers() { return m_lasers; }

private:
    bool m_active {true};
    double m_lastFireTime {0};

    std::vector<Laser> m_lasers {};
};

}
