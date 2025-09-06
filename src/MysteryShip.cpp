//
// Created by lenon on 8/29/25.
//

#include "../include/MysteryShip.h"

#include <cassert>
#include <iostream>
#include <raylib.h>

#include "Explosion.h"
#include "Game.h"

namespace SpaceInvaders {
MysteryShip::MysteryShip() {
    const auto texture = Game::GameResources->GetTexture("mystery.png");
    assert(texture.has_value());
    m_textures.push_back(texture.value());

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

    // TODO: Constrain ship to frame
    if (m_position.x < -GetTexture().width - 1 || m_position.x > GetScreenWidth() + 1) {
        Reset();
    }
}

void
MysteryShip::Draw() {
    if (!m_spawned) { return; }
    DrawTextureV(GetTexture(), m_position, WHITE);
}

void
MysteryShip::Explode() {
    Explosion e(Explosion::Type::Alien, Vector2{0, 0});

    const float xOff = m_position.x + GetTexture().width / 2 - e.GetTexture().width / 2;
    const float yOff = m_position.y + GetTexture().height / 2 - e.GetTexture().height / 2;

    e.SetPosition({xOff, yOff});
    Game::AddExplosion(e);

    Reset();
}

}
