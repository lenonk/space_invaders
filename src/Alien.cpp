#include <algorithm>
#include <string>
#include <format>

#include "Alien.h"

#include <cassert>
#include <numeric>
#include <ranges>

#include "Explosion.h"
#include "Game.h"

namespace SpaceInvaders {
Alien::Alien(const Vector2 position, const uint8_t type) {
    m_type = type;
    m_position = position;

    for (const auto i : std::views::iota(1, 2)) {
        auto texture = Game::GameResources->GetTexture(std::format("alien_{}_{}.png", std::to_string(type), i));
        assert(texture.has_value());
        m_textures.push_back(texture.value());
    }
}

void
Alien::Draw() {
    if (!Active()) { return; }

    DrawTextureV(GetNextTexture(), m_position, WHITE);
    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Draw(); });
}

void
Alien::Update() {
    if (const auto time = GetTime(); time - m_lastMoveTime > m_moveTime) {
        Move(Vector2{ m_position.x + m_speed, m_position.y });
        m_lastMoveTime = time;
    }

    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Update(); });
    std::erase_if(m_lasers, [](auto& laser) { return laser.ShouldRemove(); });
}

void
Alien::Move(const Vector2 &position) {
    m_position = position;
}

void
Alien::FireLaser() {
    const auto time = GetTime();

    const double fireDelay = static_cast<double>(GetRandomValue(AlienMinFireSpeed, AlienMaxFireSpeed)) / 1000.0f;
    if (time - m_lastFireTime < fireDelay) {
        return;
    }

    m_lastFireTime = time;
    AlienLaser l;
    l.Position({
        m_position.x + (static_cast<float>(GetTexture().width) / 2.0f) - (l.GetTexture().width / 2.0f),
        m_position.y + GetTexture().height}
    );

    m_lasers.push_back(l);
}

void
Alien::Explode() {
    m_active = false;
    auto e = std::make_unique<Explosion>(Explosion::Type::Alien, Vector2{0, 0});

    const float xOff = m_position.x + GetTexture().width / 2 - e->GetTexture().width / 2;
    const float yOff = m_position.y + GetTexture().height / 2 - e->GetTexture().height / 2;

    e->Position({xOff, yOff});
    Game::AddExplosion(e);
}

void
Alien::StepUpSpeed() {
    if (m_moveTime > 0.05f) {
        m_moveTime -= 0.05f;
    }
}

}
