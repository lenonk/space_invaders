#include <algorithm>
#include <string>
#include <format>

#include "Alien.h"

#include <numeric>
#include <ranges>

#include "Colors.h"
#include "Game.h"
#include "ParticleSystem.h"

namespace SpaceInvaders {
Alien::Alien(const Vector2 position, const uint8_t type) {
    m_type = type;
    m_position = position;

    for (const auto i : std::views::iota(1, 3)) {
        auto texture = Game::Resources->Get<Texture2D>(std::format("alien_{}_ani_{}.png", std::to_string(type), i));
        if (!texture.has_value()) {
            throw std::runtime_error(std::format("Failed to load alien texture: alien_{}_ani_{}.png", type, i));
        }

        m_textures.push_back(texture.value());
    }
}

void
Alien::Draw() const {
    if (!GetActive()) { return; }

    const auto loc_texelSize = GetShaderLocation(m_glowShader, "u_texelSize");
    const float texelSize[2] = {1.0f / static_cast<float>(GetTexture().width), 1.0f / static_cast<float>(GetTexture().height)};
    SetShaderValue(m_glowShader, loc_texelSize, texelSize, SHADER_UNIFORM_VEC2);

    DrawTextureV(GetTexture(), m_position, Colors::White);
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

    // Emit alien particle burst (tint by alien type)
    auto tint = Colors::White;
    switch (m_type) {
        case 1: tint = Colors::AlienCyan; break;
        case 2: tint = Colors::AlienPurple; break;
        case 3: tint = Colors::AlienGreen; break;
        default: break;
    }
    const Vector2 c { m_position.x + GetTexture().width / 2, m_position.y + GetTexture().height / 2 };
    Game::ParticleManager->Emit(BurstType::Alien, c, tint);
}

void
Alien::StepUpSpeed() {
    m_moveTime = std::max(m_moveTime - 0.035f, 0.04f);
}

}
