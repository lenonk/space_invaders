#include "Laser.h"

#include <cassert>
#include <iostream>
#include <ranges>

#include "Colors.h"
#include "Explosion.h"
#include "Game.h"

namespace SpaceInvaders {

Laser::Laser(const Type type) {
    assert(type != Type::None);
    if (type == Type::Alien) {
        for (const auto i : std::views::iota(1, 5)) {
            auto texture = Game::GameResources->GetTexture(std::format("alien_laser_{}.png", std::to_string(i)));
            if (!texture.has_value()) {
                throw std::runtime_error(std::format("Failed to load alien laser texture: alien_laser_{}.png", i));
            }
            m_textures.push_back(texture.value());
        }

        const auto sound = Game::GameResources->GetSound("laser.ogg");
        if (!sound.has_value()) {
            throw std::runtime_error("Failed to load alien laser sound");
        }
        m_sounds.push_back(sound.value());

        m_textureSwapTime = 0.125;
        m_speed = -AlienSpeed;
    } else if (type == Type::Player) {
        const auto texture = Game::GameResources->GetTexture("player_laser_1.png");
        if (!texture.has_value()) {
            throw std::runtime_error("Failed to load player laser texture");
        }
        m_textures.push_back(texture.value());

        const auto sound = Game::GameResources->GetSound("laser.ogg");
        if (!sound.has_value()) {
            throw std::runtime_error("Failed to load player laser sound");
        }
        m_sounds.push_back(sound.value());

    }

    m_type = type;
    PlaySound(Entity::GetNextSound());
}

void
Laser::Update() {
    if (GetPosition().y <= 0 || GetPosition().y >= GetScreenHeight() - Game::ScreenPadding * 2) {
        Explode();
        return;
    }

    if (const auto time = GetTime(); time - m_lastTextureSwapTime > m_textureSwapTime) {
        GetNextTexture();
        m_lastTextureSwapTime = time;
    }
    m_position.y += m_speed * GetFrameTime();
}

void
Laser::Draw() {
    const auto tex = GetTexture();
    DrawTextureV(tex, m_position, WHITE);
}

void
Laser::Explode(const bool createExplosion) {
    if (!GetActive()) { return; }

    SetActive(false);
    if (!createExplosion) { return; }

    Explosion e(Explosion::Type::Laser, {0, 0});
    e.SetPosition({GetPosition().x + GetTexture().width / 2 - e.GetTexture().width / 2, GetPosition().y});
    Game::AddExplosion(e);
}

Vector2
Laser::GetPosition() const {
    if (m_type == Type::Alien) {
        return {m_position.x, m_position.y + GetTexture().height};
    }

    return m_position;
}

Laser::Type
Laser::GetType() const {
    return m_type;
}
}
