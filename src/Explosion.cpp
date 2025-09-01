#include <algorithm>
#include <ranges>

#include "Explosion.h"

#include <cassert>
#include <iostream>

#include "Game.h"
#include "Laser.h"

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
    assert(texture.has_value());
    m_textures.push_back(texture.value());

    const auto sound = Game::GameResources->GetSound(soundName);
    assert(sound.has_value());
    m_sounds.push_back(sound.value());

    m_createdTime = GetTime();

    m_position = position;
    PlaySound(Entity::GetSound());
}

void
Explosion::Draw() {
    DrawTextureV(GetTexture(), m_position, WHITE);
}

bool
Explosion::IsExpired() const {
    const auto time = GetTime();
    return time - m_createdTime > m_ttl[m_type];
}

}
