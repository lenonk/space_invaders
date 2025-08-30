//
// Created by lenon on 8/29/25.
//

#include "../include/MysteryShip.h"

#include <iostream>
#include <ostream>
#include <raylib.h>

SpaceInvaders::MysteryShip::MysteryShip() {
    m_lastSpawnTime = nextSpawnTime = GetRandomValue(1, SpawnInterval);
    m_texture = LoadTexture("Graphics/mystery.png");
}

SpaceInvaders::MysteryShip::~MysteryShip() {
    UnloadTexture(m_texture);
}

void
SpaceInvaders::MysteryShip::CheckSpawn() {
    static int temp = 0;
    if (spawned) { return; }
    const auto time = GetTime();
    if (time - m_lastSpawnTime < nextSpawnTime) { return; }

    println(std::cout, "MysteryShip {} spawned!", temp++);

    m_lastSpawnTime = time;
    m_direction = GetRandomValue(0, 1) ? 1 : -1;
    if (m_direction > 0) {
        m_position.x = -m_texture.width;
        m_speed = Speed;
    }
    else {
        m_position.x = GetScreenWidth();
        m_speed = -Speed;
    }
    spawned = true;
    nextSpawnTime = GetRandomValue(1, SpawnInterval);
}

void
SpaceInvaders::MysteryShip::Update() {
    CheckSpawn();
    if (!spawned) { return; }

    m_position.x += m_speed * GetFrameTime();

    if (m_position.x < -m_texture.width - 1 || m_position.x > GetScreenWidth() + 1) {
        spawned = false;
    }
}

void
SpaceInvaders::MysteryShip::Draw() const {
    if (!spawned) { return; }
    std::println(std::cout,"Drawing M-Ship at: {}, {} S: {} D: {}", m_position.x, m_position.y, m_speed, m_direction);
    DrawTextureV(m_texture, m_position, WHITE);
}
