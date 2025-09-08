#include <algorithm>
#include <ranges>

#include "Explosion.h"

#include <iostream>

#include "Game.h"

namespace SpaceInvaders {
Explosion::Explosion(const Type type, const Vector2 &position) : m_type(type) {
    std::string textureName {};
    std::string soundName   {};
    switch (type) {
        case Type::Laser:
            textureName = "laser_explosion.png";
            soundName = "explosion.ogg";
            m_ttl[type] = LaserTTL;
            break;
        case Type::Alien:
            textureName = "alien_explosion.png";
            soundName = "explosion.ogg";
            m_ttl[type] = AlienTTL;
            break;
        default:
            break;
    }

    const auto texture = Game::GameResources->GetTexture(textureName);
    if (!texture.has_value()) {
        throw std::runtime_error(std::format("Failed to load texture: {}", textureName));
    }
    m_textures.push_back(texture.value());

    const auto sound = Game::GameResources->GetSound(soundName);
    if (!sound.has_value()) {
        throw std::runtime_error(std::format("Failed to load sound: {}", soundName));
    }
    m_sounds.push_back(sound.value());

    m_createdTime = GetTime();

    m_position = position;
    PlaySound(Entity::GetSound());
}

void
Explosion::Draw() const {
    DrawTextureV(GetTexture(), m_position, WHITE);
}

bool
Explosion::IsExpired() const {
    const auto time = GetTime();
    return time - m_createdTime > m_ttl[m_type];
}

}
