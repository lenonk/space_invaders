#pragma once
#include <memory>
#include <raylib.h>
#include <vector>

#include "Laser.h"

namespace SpaceInvaders {

constexpr float PlayerSpeed = 420.0f;
constexpr float PlayerFireSpeed = 0.2f;
constexpr uint8_t MaxLasers = 5;

class SpaceShip {
public:
    SpaceShip();
    ~SpaceShip();

    void Update();
    void Draw() const;

    void MoveLeft();
    void MoveRight();

    void FireLaser();

private:
    double m_lastFireTime {0};

    Texture2D m_texture {};
    Vector2 m_position {0, 0};

    std::vector<Laser> m_lasers {};
};

}
