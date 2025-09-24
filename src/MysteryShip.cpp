//
// Created by lenon on 8/29/25.
//

#include "../include/MysteryShip.h"

#include <iostream>
#include <raylib.h>

#include "Game.h"
#include "ParticleSystem.h"

namespace SpaceInvaders {
MysteryShip::MysteryShip() {
    for (const auto i : std::views::iota(1, 2)) {
        auto texture = Game::Resources->Get<Texture2D>(std::format("mystery_ani_{}.png", i));
        if (!texture.has_value()) {
            throw std::runtime_error(std::format("Failed to load spaceship texture: mystery_ani_{}.png", i));
        }

        m_textures.push_back(texture.value());
    }

    m_lastSpawnTime = nextSpawnTime = GetRandomValue(5, SpawnInterval);
    Reset();
}

void
MysteryShip::Reset() {
    m_spawned = false;
    m_position = {-1000.0f, -1000.0f};
    m_lastSpawnTime = GetTime();;
}

void
MysteryShip::CheckSpawn() {
    if (m_spawned) { return; }

    const auto time = GetTime();
    if (time - m_lastSpawnTime < nextSpawnTime) { return; }

    m_lastSpawnTime = time;
    m_direction = GetRandomValue(0, 1) ? 1 : -1;
    if (m_direction > 0) {
        m_position.x  = -GetTexture().width;
        m_speed = Speed;
    }
    else {
        m_position.x = GetScreenWidth();
        m_speed = -Speed;
    }
    m_position.y = yVal;
    m_spawned = true;
    nextSpawnTime = GetRandomValue(5, SpawnInterval);
}

void
MysteryShip::Update() {
    CheckSpawn();
    if (!m_spawned) { return; }

    m_position.x += m_speed * GetFrameTime();
    if ((int32_t)GetFrameTime() % GetFPS() == 0)
        GetNextTexture();

    if (m_position.x < -GetTexture().width - 1 || m_position.x > GetScreenWidth() + 1) {
        Reset();
    }
}

void
MysteryShip::Draw() const {
    if (!m_spawned) { return; }
    DrawTextureV(GetTexture(), m_position, WHITE);
}

void
MysteryShip::Explode() {
    const Vector2 c { m_position.x + GetTexture().width  / 2, m_position.y + GetTexture().height / 2 };

    Game::ParticleManager->Emit(BurstType::Mystery, c);

    Reset();
}

}
