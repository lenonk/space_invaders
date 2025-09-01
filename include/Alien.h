#pragma once

#include <vector>
#include <cstdint>

#include <raylib.h>

#include "Laser.h"

namespace SpaceInvaders {

class Alien final : public Entity {
public:
    static constexpr uint8_t AlienTypes { 3 };
    static constexpr float AlienSpeed = 10.0f;
    static constexpr float AlienMinFireSpeed = 750.0f;
    static constexpr float AlienMaxFireSpeed = 5000.0f;

    Alien(Vector2 position, uint8_t type);
    // TODO: Check if we still need these
    Alien(const Alien &other) = default;
    Alien &operator=(const Alien &other) = default;
    Alien() = default;
    ~Alien() override = default;

    void Draw() override;
    void Update() override;
    void Move(const Vector2 &position);
    void FireLaser();
    void Explode();

    static void StepUpSpeed();

    [[nodiscard]] float GetSpeed() const { return m_speed; }
    [[nodiscard]] bool Active() const { return m_active; }
    [[nodiscard]] float Speed() const { return m_speed; }
    [[nodiscard]] uint8_t Type() const { return m_type; }
    [[nodiscard]] std::vector<AlienLaser> &GetLasers() { return m_lasers; }

    void SetSpeed(const float speed) { m_speed = speed; }

private:
    bool m_active           {true};
    uint8_t m_type          {0};
    float m_speed           {AlienSpeed};
    double m_lastMoveTime   {0.0f};

    std::vector<AlienLaser> m_lasers    {};

    inline static double m_lastFireTime {0};
    inline static float m_moveTime = 0.75f;

};

}