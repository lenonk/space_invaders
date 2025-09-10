#pragma once

#include <vector>
#include <cstdint>

#include <raylib.h>

#include "Laser.h"

namespace SpaceInvaders {

class Alien final : public Entity {
public:
    static constexpr uint8_t AlienTypes { 3 };

    Alien(Vector2 position, uint8_t type);
    Alien() = default;
    ~Alien() override = default;

    void Draw() const override;
    void Update() override;
    void Move(const Vector2 &position);
    void FireLaser() const;
    void Explode();
    void SetSpeed(const float speed) { m_speed = speed; }

    [[nodiscard]] float GetSpeed() const { return m_speed; }
    [[nodiscard]] uint8_t GetType() const { return m_type; }

    static void StepUpSpeed();
    static void ResetSpeed() { m_moveTime = MoveTime; };

private:
    static constexpr float MoveTime = 0.75f;

    const float Speed = 10.0f;
    const float MinFireSpeed = 750.0f;
    const float MaxFireSpeed = 5000.0f;

    uint8_t m_type          {0};
    float m_speed           {Speed};
    double m_lastMoveTime   {0.0f};

    inline static double m_lastFireTime {0};
    inline static float m_moveTime = MoveTime;
};

}