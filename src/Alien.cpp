#include <algorithm>
#include <string>
#include <format>

#include "Alien.h"

#include <numeric>
#include <ranges>

#include "Explosion.h"
#include "Game.h"
#include "Logger.h"

namespace SpaceInvaders {
Alien::Alien(const Vector2 position, const uint8_t type) {
    m_type = type;
    m_position = position;

    for (const auto i : std::views::iota(1, 3)) {
        auto texture = Game::Resources->GetTexture(std::format("alien_{}_ani_{}.png", std::to_string(type), i));
        if (!texture.has_value()) {
            throw std::runtime_error(std::format("Failed to load alien texture: alien_{}_ani_{}.png", type, i));
        }
        m_textures.push_back(texture.value());
    }
}

void
Alien::Draw() const {
    if (!GetActive()) { return; }

    DrawTextureV(GetTexture(), m_position, WHITE);
}

void
Alien::Update() {
    if (const auto time = GetTime(); time - m_lastMoveTime > m_moveTime) {
        Move(Vector2{ m_position.x + m_speed, m_position.y });
        GetNextTexture();
        m_lastMoveTime = time;
    }
}

void
Alien::Move(const Vector2 &position) {
    m_position = position;
}

void
Alien::FireLaser() const {
    const auto time = GetTime();

    const double fireDelay = static_cast<double>(GetRandomValue(MinFireSpeed, MaxFireSpeed)) / 1000.0f;
    if (time - m_lastFireTime < fireDelay) {
        return;
    }

    m_lastFireTime = time;
    const auto l = std::make_shared<AlienLaser>();
    l->SetPosition({
        m_position.x + (static_cast<float>(GetTexture().width) / 2.0f) - (l->GetTexture().width / 2.0f),
        m_position.y + GetTexture().height}
    );

    Game::AddAlienLaser(l);
}

void
Alien::Explode() {
    SetActive(false);
    Explosion e(Explosion::Type::Alien, Vector2{0, 0});

    const float xOff = m_position.x + GetTexture().width / 2 - e.GetTexture().width / 2;
    const float yOff = m_position.y + GetTexture().height / 2 - e.GetTexture().height / 2;

    e.SetPosition({xOff, yOff});
    Game::AddExplosion(e);
}

void
Alien::StepUpSpeed() {
    if (m_moveTime > 0.05f) {
        m_moveTime -= 0.0135;
    }
}

}
