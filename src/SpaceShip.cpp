#include "SpaceShip.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ostream>

#include "Explosion.h"
#include "Game.h"
#include "Laser.h"

namespace SpaceInvaders {
SpaceShip::SpaceShip() {
    const auto texture = Game::Resources->GetTexture("spaceship.png");
    assert(texture.has_value());
    m_textures.push_back(texture.value());

    Reset();
}

void
SpaceShip::Update() {
    std::erase_if(m_lasers, [](auto &laser) { return !laser.GetActive(); });
    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Update(); });

    if (!m_active) {
        if (m_respawnTimer > 0 && GetTime() - m_respawnTimer > RespawnTime) {
            m_respawnTimer = 0;
            Reset();
        }
        return;
    }

    if (m_invulnerable && GetTime() - m_invulnerableTimer > InvulnerableTime) {
        m_invulnerable = false;
    }
}

void
SpaceShip::Draw() const {
    if (!m_active) { return; }

    for (const auto &laser : m_lasers) { laser.Draw(); }

    if (!m_invulnerable || static_cast<int64_t>(GetTime() * 10) % 2 == 0)
        DrawTextureV(GetTexture(), m_position, WHITE);
}

void
SpaceShip::Reset() {
    m_position = { (GetScreenWidth() - Entity::GetTexture().width) / 2.0f, // X
                   Game::GroundLevel - Entity::GetTexture().height - 2 };    // Y
    m_active = true;
    m_invulnerable = true;
    m_invulnerableTimer = GetTime();
}

void
SpaceShip::MoveLeft() {
    m_position.x -= GetFrameTime() * Speed;
    if (m_position.x < Game::ScreenPadding / 2.0f) { m_position.x = Game::ScreenPadding / 2.0f; }
}

void
SpaceShip::MoveRight() {
    m_position.x += GetFrameTime() * Speed;
    if (m_position.x > GetScreenWidth() - GetTexture().width - Game::ScreenPadding / 2.0f) {
        m_position.x = GetScreenWidth() - GetTexture().width - Game::ScreenPadding / 2.0f;
    }
}

bool
SpaceShip::Die() {
    if (m_invulnerable) { return false; }
    m_active = false;

    Explosion e(Explosion::Type::Alien, Vector2 {0, 0});
    const float xOff = m_position.x + GetTexture().width / 2 - e.GetTexture().width / 2;
    const float yOff = m_position.y + GetTexture().height / 2 - e.GetTexture().height / 2;
    e.SetPosition({xOff, yOff});

    Game::AddExplosion(e);

    m_respawnTimer = GetTime();
    m_invulnerable = true;

    return true;
}

void
SpaceShip::FireLaser() {
    const auto time = GetTime();
    if (time - m_lastFireTime < FireSpeed || m_lasers.size() >= MaxLasers)
        return;

    m_lastFireTime = time;
    PlayerLaser l;
    l.SetPosition({
        m_position.x + GetTexture().width / 2.0f - l.GetTexture().width / 2.0f,
        m_position.y}
    );
    m_lasers.push_back(l);
}

}
