#pragma once
#include <map>
#include <raylib.h>

#include "Entity.h"

namespace SpaceInvaders {

class Explosion final : public Entity {
public:
    static constexpr float LaserTTL = 0.5f;
    static constexpr float AlienTTL = 0.25f;

    enum class Type {
        None,
        Laser,
        Alien,
    };

    explicit Explosion(Type type, const Vector2 &position);
    ~Explosion() override = default;

    void Draw() override;

    [[nodiscard]] bool IsExpired() const;

private:
    double m_createdTime {0.0f};
    Type m_type          {Type::None};

    static inline std::map<Type, double> m_ttl {{Type::Laser, LaserTTL}, {Type::Alien, AlienTTL}};
};

}
