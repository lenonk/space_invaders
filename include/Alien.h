#pragma once

#include <array>
#include <vector>
#include <cstdint>

#include <raylib.h>

#include "Laser.h"

namespace SpaceInvaders {

class Alien {
public:
    enum Direction : int8_t {
        Left = -1,
        Right = 1,
    };

    static constexpr uint8_t NumTextures { 3 };
    static constexpr float MoveTime = 0.75f;
    static constexpr float Speed = 10.0f;
    static constexpr float AlienMinFireSpeed = 750.0f;
    static constexpr float AlienMaxFireSpeed = 5000.0f;

    Alien(Vector2 position, uint8_t type);
    Alien(const Alien &other) = default;
    Alien &operator=(const Alien &other) = default;
    Alien() = default;
    ~Alien() = default;

    void Draw() const;
    void Update();
    void Move(const Vector2 &position);
    void FireLaser();

    static std::array<Texture2D, 2> &LoadTexture(uint8_t type);
    static void UnloadTextures();

    [[nodiscard]] Vector2 Position() const { return m_position; }
    [[nodiscard]] Texture2D &Texture() { return m_texture[m_frame]; }
    [[nodiscard]] uint8_t Type() const { return m_type; }
    [[nodiscard]] static int8_t Direction() { return m_direction; }

    static auto Direction(const int8_t dir) { m_direction = dir; }
private:
    uint8_t m_type          {0};
    double m_lastMoveTime   {0};
    Vector2 m_position      {0, 0};

    std::vector<Laser> m_lasers            {};
    std::array<Texture2D, 2> m_texture     {};

    inline static int8_t m_direction      {Right};
    inline static int8_t m_frame          {0};
    inline static double m_lastFireTime   {0};
    inline static std::array<std::array<Texture2D, 2>, NumTextures> m_textures {};
};

}