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
    virtual void Draw() = 0;

    [[nodiscard]] virtual bool GetActive() const;
    [[nodiscard]] virtual Vector2 GetPosition() const;
    [[nodiscard]] virtual Rectangle GetRect() const;
    [[nodiscard]] virtual Sound &GetSound();
    [[nodiscard]] virtual Texture2D GetTexture() const;

    virtual Sound &GetNextSound();
    virtual Texture2D &GetNextTexture();

    virtual void SetPosition(const Vector2 &position);
    virtual void SetActive(bool active);

    [[nodiscard]] virtual bool CollidesWith(const Rectangle &other);
    [[nodiscard]] virtual bool CollidesWith(Entity &other);

    template<std::ranges::range Container>
    requires std::convertible_to<std::ranges::range_value_t<Container>, std::shared_ptr<Entity>>
    [[nodiscard]] std::shared_ptr<Entity> CollidesWithAny(const Container &others) {
        for (const auto &other : others) {
            if (CollidesWith(*other)) {
                return other;
            }
        }
        return nullptr;
    }

protected:
    bool m_active                       {true};
    Vector2 m_position                  {};
    uint8_t m_textureIdx                {0};
    uint8_t m_soundIdx                  {0};
    std::vector<Texture2D> m_textures   {};
    std::vector<Sound> m_sounds         {};
};

}
