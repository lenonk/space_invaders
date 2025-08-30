#include <algorithm>
#include <string>
#include <format>

#include "Alien.h"

#include <cassert>
#include <iostream>
#include <ostream>
#include <ranges>

namespace SpaceInvaders {
Alien::Alien(const Vector2 position, const uint8_t type) : m_type(type), m_position(position) {
    m_texture = LoadTexture(type);
}

void
Alien::Draw() const {
    DrawTextureV(m_texture[m_frame], m_position, WHITE);
    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Draw(); });
}

void
Alien::Update() {
    const auto time = GetTime();
    if (time - m_lastMoveTime > MoveTime) {
        Move(Vector2{ m_position.x + Speed * m_direction, m_position.y });
        m_frame = (m_frame + 1) % 2;
        m_lastMoveTime = time;
    }

    std::ranges::for_each(m_lasers, [](auto &laser) { laser.Update(Laser::AlienLaserSpeed); });
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
    m_lasers.emplace_back(Vector2{
            m_position.x + (static_cast<float>(m_texture[m_frame].width) / 2.0f) - (DefaultLaserWidth / 2.0f),
            m_position.y + m_texture[m_frame].height + 1}
    );
}

std::array<Texture2D, 2> &
Alien::LoadTexture(const uint8_t type) {
    const uint8_t type_idx = type - 1;
    if (m_textures[type_idx][0].id == 0) {
        m_textures[type_idx][0] = ::LoadTexture(std::format("Graphics/alien_{}_ani_1.png", std::to_string(type)).c_str());
        m_textures[type_idx][1] = ::LoadTexture(std::format("Graphics/alien_{}_ani_2.png", std::to_string(type)).c_str());
    }

    return m_textures[type_idx];
}

void
Alien::UnloadTextures() {
    std::ranges::for_each(m_textures, [](auto &texture) {
        if (texture[0].id != 0) { UnloadTexture(texture[0]); }
        if (texture[1].id != 0) { UnloadTexture(texture[1]); }
    });
}

}
