#include "Entity.h"

#include "Game.h"

namespace SpaceInvaders {

void
Entity::Update() {
}

bool
Entity::GetActive() const {
   return m_active;
}

Vector2
Entity::GetPosition() const {
    return m_position;
}

Rectangle
Entity::GetRect() const {
    if (m_active) {
        return {
            m_position.x,
            m_position.y,
            static_cast<float>(GetTexture().width),
            static_cast<float>(GetTexture().height)
        };
    }

    return {};
}

Texture2D
Entity::GetTexture() const {
    return m_textures[m_textureIdx];
}

Texture2D &
Entity::GetNextTexture() {
    m_textureIdx++;
    if (m_textureIdx >= m_textures.size()) {
        m_textureIdx = 0;
    }
    return m_textures[m_textureIdx];
}

void
Entity::SetPosition(const Vector2 &position) {
    m_position = position;
}

void
Entity::SetActive(const bool active) {
    m_active = active;
}

Sound &
Entity::GetSound() {
    return m_sounds[m_soundIdx];
}

Sound &
Entity::GetNextSound() {
    m_soundIdx++;
    if (m_soundIdx >= m_sounds.size()) {
        m_soundIdx = 0;
    }
    return m_sounds[m_soundIdx];
}

bool
Entity::CollidesWith(const Rectangle &other) {
    return CheckCollisionRecs(GetRect(), other);
}

bool
Entity::CollidesWith(Entity &other) {
    return CollidesWith(other.GetRect());
}

}