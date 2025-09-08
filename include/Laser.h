#pragma once
#include <raylib.h>

#include "Entity.h"

namespace SpaceInvaders {

class Laser : public Entity {
public:
    Laser() = default;
    ~Laser() override = default;

    void Update() override;
    void Draw() const override;
    void Explode(bool createExplosion = true);

    [[nodiscard]] Vector2 GetPosition() const override;

protected:
    float m_speed               {0.0f};
    float m_lastTextureSwapTime {0.0f};
    float m_textureSwapTime     {0.0f};

    virtual void LoadResources() = 0;
    virtual bool IsOutOfBounds() const;
};

// Player laser implementation
class PlayerLaser final : public Laser {
public:
    static constexpr float Speed = -520.0f;

    PlayerLaser();
    ~PlayerLaser() override = default;

protected:
    void LoadResources() override;
};

// Alien laser implementation
class AlienLaser final : public Laser {
public:
    static constexpr float Speed = 420.0f;
    static constexpr float TextureSwapTime = 0.125f;

    AlienLaser();
    ~AlienLaser() override = default;

    [[nodiscard]] Vector2 GetPosition() const override;

protected:
    void LoadResources() override;
};

}
