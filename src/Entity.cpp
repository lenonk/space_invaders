#include "Entity.h"

#include "Game.h"

namespace SpaceInvaders {

void
Entity::Update() {
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

const Texture2D &
Entity::GetNextTexture() const {
    m_textureIdx++;
    if (m_textureIdx >= m_textures.size()) {
        m_textureIdx = 0;
    }
    return m_textures[m_textureIdx];
}

const Sound &
Entity::GetNextSound() const {
    m_soundIdx++;
    if (m_soundIdx >= m_sounds.size()) {
        m_soundIdx = 0;
    }
    return m_sounds[m_soundIdx];
}

}