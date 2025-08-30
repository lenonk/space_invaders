#pragma once
#include <cstdint>

#include <raylib.h>

namespace SpaceInvaders {

constexpr uint8_t DefaultLaserWidth = 4;
constexpr uint8_t DefaultLaserLength = 15;

constexpr double ExplosionLingerTime = 0.50f;

class Laser {
public:
    static constexpr float PlayerLaserSpeed = 420.0f;
    static constexpr float AlienLaserSpeed = -420.0f;

    explicit Laser(Vector2 position);
    ~Laser() = default;

    void Update(const float speed);
    void Draw() const;

    [[nodiscard]] Vector2 GetPosition() const { return m_position; }
    [[nodiscard]] bool ShouldRemove();

    static void UnloadTexture();

private:
    double m_explodeTime {0};
    double m_explosionLingerTime {ExplosionLingerTime};
    uint8_t m_frame {0};
    Vector2 m_position {0, 0};
    inline static Texture2D m_explodedtexture {};
};

}
