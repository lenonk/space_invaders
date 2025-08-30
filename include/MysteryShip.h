#pragma once
#include <cstdint>
#include <raylib.h>

namespace SpaceInvaders {

class MysteryShip {
public:
    static constexpr float Speed = 150.0f;
    static constexpr double SpawnInterval = 25.0f;

    MysteryShip();
    ~MysteryShip();

    void CheckSpawn();
    void Update();
    void Draw() const;

private:
    bool spawned {false};
    int8_t m_direction {1};
    float m_speed {Speed};
    Vector2 m_position {0, 50};
    double m_lastSpawnTime {0};
    double nextSpawnTime {0};

    Texture2D m_texture {};
};

}
