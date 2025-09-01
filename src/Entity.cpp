#include "Entity.h"
#include "Game.h"

namespace SpaceInvaders {

void
Entity::Update() {
}

Vector2 &
Entity::GetPosition() {
    return m_position;
}

Rectangle
Entity::GetRect() {
    return {
        m_position.x,
        m_position.y,
        static_cast<float>(GetTexture().width),
        static_cast<float>(GetTexture().height)
    };
}

Texture2D &
Entity::GetTexture() {
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