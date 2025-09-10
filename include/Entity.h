#pragma once
#include <cstdint>
#include <memory>
#include <raylib.h>
#include <vector>
#include <concepts>

namespace SpaceInvaders {

class Entity {
public:
    Entity() = default;
    virtual ~Entity() = default;

    virtual void Update();
    virtual void Draw() const = 0;

    [[nodiscard]] virtual bool GetActive() const                { return m_active; }
    [[nodiscard]] virtual const Vector2 &GetPosition() const    { return m_position; }
    [[nodiscard]] virtual const Sound &GetSound() const         { return m_sounds[m_soundIdx]; }
    [[nodiscard]] virtual const Texture2D &GetTexture() const   { return m_textures[m_textureIdx]; }
    [[nodiscard]] virtual Rectangle GetRect() const;

    virtual const Sound &GetNextSound() const;
    virtual const Texture2D &GetNextTexture() const;

    virtual void SetPosition(const Vector2 &position) { m_position = position; }
    virtual void SetActive(const bool active) { m_active = active; }

    template<std::ranges::range Container>
    requires std::convertible_to<std::ranges::range_value_t<Container>, std::shared_ptr<Entity>>
    [[nodiscard]] std::shared_ptr<Entity> CollidesWithAny(const Container &others);

    [[nodiscard]] virtual bool CollidesWith(const Rectangle &other) { return CheckCollisionRecs(GetRect(), other); }
    [[nodiscard]] virtual bool CollidesWith(Entity &other) { return GetActive() && CollidesWith(other.GetRect()); }

protected:
    bool m_active                       {true};
    Vector2 m_position                  {};

    mutable uint8_t m_textureIdx        {0};
    mutable uint8_t m_soundIdx          {0};

    std::vector<Texture2D> m_textures   {};
    std::vector<Sound> m_sounds         {};
};

template<std::ranges::range Container>
requires std::convertible_to<std::ranges::range_value_t<Container>, std::shared_ptr<Entity>>
std::shared_ptr<Entity>
Entity::CollidesWithAny(const Container &others) {
    for (const auto &other : others) {
        if (CollidesWith(*other)) {
            return other;
        }
    }
    return nullptr;
}

}
