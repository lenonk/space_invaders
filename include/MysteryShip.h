#pragma once

#include "Entity.h"

namespace SpaceInvaders {

class MysteryShip : public Entity {
public:
    static constexpr float Speed = 150.0f;
    static constexpr float yVal = 50.0f;
    static constexpr double SpawnInterval = 25.0f;

    MysteryShip();
    ~MysteryShip() override = default;

    void CheckSpawn();
    void Update() override;
    void Draw() override;
    void Explode();
    void Reset();

private:
    bool m_spawned {false};
    int8_t m_direction {1};
    float m_speed {Speed};
    double m_lastSpawnTime {0};
    double nextSpawnTime {0};
};

}
