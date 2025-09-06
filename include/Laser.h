#pragma once
#include <raylib.h>

#include "Entity.h"

namespace SpaceInvaders {

class Laser final : public Entity{
public:
    static constexpr float PlayerSpeed = -520.0f;
    static constexpr float AlienSpeed = -420.0f;

    enum class Type {
        None,
        Player,
        Alien
    };

    explicit Laser(Type type);
    ~Laser() override = default;

    void Update() override;
    void Draw() override;
    void Explode(bool createExplosion = true);

    [[nodiscard]] Vector2 GetPosition() const override;
    [[nodiscard]] Type GetType() const;

protected:
    Type m_type                 {Type::None};
    float m_lastTextureSwapTime {0};
    float m_textureSwapTime     {0};
    float m_textureSwapTimeMax  {0.1f};
    float m_speed               {PlayerSpeed};
};

}
