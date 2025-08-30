#include "SpaceShip.h"

#include <algorithm>

#include "Laser.h"

namespace SpaceInvaders {
SpaceShip::SpaceShip() {
    m_texture = LoadTexture("Graphics/spaceship.png");
    m_position = { static_cast<float>(GetScreenWidth() - m_texture.width) / 2.0f, // X
                   static_cast<float>(GetScreenHeight()) - static_cast<float>(m_texture.height) }; // Y
}

SpaceShip::~SpaceShip() {
    UnloadTexture(m_texture);
}

void
SpaceShip::Update() {
    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Update(Laser::PlayerLaserSpeed); });
    std::erase_if(m_lasers, [](auto& laser) {
        return laser.ShouldRemove();
    });
}

void
SpaceShip::Draw() const {
    DrawTextureV(m_texture, m_position, WHITE);
    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Draw(); });
}

void
SpaceShip::MoveLeft() {
    m_position.x -= GetFrameTime() * PlayerSpeed;
    if (m_position.x < 0) { m_position.x = 0; }
}

void
SpaceShip::MoveRight() {
    m_position.x += GetFrameTime() * PlayerSpeed;
    if (m_position.x > static_cast<float>(GetScreenWidth()) - static_cast<float>(m_texture.width)) {
        m_position.x = static_cast<float>(GetScreenHeight()) - static_cast<float>(m_texture.width);
    }
}

void
SpaceShip::FireLaser() {
    const auto time = GetTime();
    if (time - m_lastFireTime < PlayerFireSpeed || m_lasers.size() >= MaxLasers)
        return;

    m_lastFireTime = time;
    m_lasers.emplace_back(Vector2{
            m_position.x + (static_cast<float>(m_texture.width) / 2.0f) - (DefaultLaserWidth / 2.0f),
            m_position.y + 1}
    );
}

}
