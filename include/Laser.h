#pragma once
#include <raylib.h>

#include "Entity.h"

namespace SpaceInvaders {

class Laser : public Entity{
public:
    static constexpr float Speed = -420.0f;

    Laser();
    ~Laser() override = default;

    void Update() override;
    void Draw() override;
    void Explode(bool createExplosion = true);

    void Position(const Vector2 &position) { m_position = position; }
    [[nodiscard]] bool ShouldRemove() const;

private:
    float m_speed {Speed};
    bool m_active {true};
    Vector2 m_position {0, 0};
};

class AlienLaser final : public Laser {
public:
    static constexpr float Speed = 420.0f;
    AlienLaser();

private:
    float m_speed {Speed};
};

}
