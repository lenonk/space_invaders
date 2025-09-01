#include "Laser.h"

#include <cassert>
#include <iostream>
#include <ranges>

#include "Colors.h"
#include "Explosion.h"
#include "Game.h"

namespace SpaceInvaders {

AlienLaser::AlienLaser() {
    for (const auto i : std::views::iota(1, 2)) {
        auto texture = Game::GameResources->GetTexture(std::format("alien_laser_{}_{}.png", std::to_string(i), i));
        assert(texture.has_value());
        m_textures.push_back(texture.value());
    }

    const auto sound = Game::GameResources->GetSound("laser.ogg");
    assert(sound.has_value());
    m_sounds.push_back(sound.value());

    PlaySound(Entity::GetNextSound());
}

Laser::Laser() {
    const auto texture = Game::GameResources->GetTexture("player_laser_1.png");
    assert(texture.has_value());
    m_textures.push_back(texture.value());

    const auto sound = Game::GameResources->GetSound("laser.ogg");
    assert(sound.has_value());
    m_sounds.push_back(sound.value());

    PlaySound(Entity::GetNextSound());
}

void
Laser::Update() {
    if (m_position.y < 0 || m_position.y - GetTexture().width > GetScreenHeight()) {
        Explode();
        return;
    }
    m_position.y += m_speed * GetFrameTime();
}

void
Laser::Draw() {
    const auto tex = GetNextTexture();
    DrawTextureV(tex, m_position, WHITE);
}

void
Laser::Explode(const bool createExplosion) {
    m_active = false;
    if (!createExplosion) { return; }

    auto e = std::make_unique<Explosion>(Explosion::Type::Laser, m_position);
    Game::AddExplosion(e);
}

bool
Laser::ShouldRemove() const {
    return !m_active;
}
}
