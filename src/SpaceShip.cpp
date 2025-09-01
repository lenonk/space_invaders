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
    const auto texture = Game::GameResources->GetTexture("spaceship.png");
    assert(texture.has_value());
    m_textures.push_back(texture.value());

    m_position = { static_cast<float>(GetScreenWidth() - Entity::GetTexture().width) / 2.0f, // X
                   static_cast<float>(GetScreenHeight()) - static_cast<float>(Entity::GetTexture().height) - 100 }; // Y
}

void
SpaceShip::Update() {
    std::erase_if(m_lasers, [](auto& laser) {
        return laser.ShouldRemove();
    });

    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Update(); });

    if (!m_active)
        return;

    if (IsKeyDown(KEY_LEFT))
        MoveLeft();
    else if (IsKeyDown(KEY_RIGHT))
        MoveRight();
    else if (IsKeyDown(KEY_SPACE))
        FireLaser();
}

void
SpaceShip::Draw() {
    if (m_active) {
        DrawTextureV(GetTexture(), m_position, WHITE);
        std::ranges::for_each(m_lasers, [](auto &laser) { laser.Draw(); });
    }
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

void
SpaceShip::Explode() {
    m_active = false;

    auto e = std::make_unique<Explosion>(Explosion::Type::Alien, Vector2 {0, 0});
    const float xOff = m_position.x + GetTexture().width / 2 - e->GetTexture().width / 2;
    const float yOff = m_position.y + GetTexture().height / 2 - e->GetTexture().height / 2;
    e->Position({xOff, yOff});

    Game::AddExplosion(e);

    // TODO: Repawn the player after a short delay in the center of the screen with invulnerability
}

void
SpaceShip::FireLaser() {
    const auto time = GetTime();
    if (time - m_lastFireTime < FireSpeed || m_lasers.size() >= MaxLasers)
        return;

    m_lastFireTime = time;
    Laser l;
    l.Position({
        m_position.x + GetTexture().width / 2.0f - l.GetTexture().width / 2.0f,
        m_position.y}
    );
    m_lasers.push_back(l);
}

}
