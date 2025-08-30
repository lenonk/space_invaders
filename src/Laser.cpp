#include "Laser.h"

#include <iostream>

#include "Colors.h"

namespace SpaceInvaders {

Laser::Laser(const Vector2 position) : m_position(position) {
    if (m_explodedtexture.id == 0) {
        m_explodedtexture = LoadTexture("Graphics/explosion.png");
    }
}

void
Laser::Update(const float speed) {
    if (m_explodeTime != 0.0f) return;
    m_position.y -= speed * GetFrameTime();

}

bool
Laser::ShouldRemove() {
    if (m_position.y < 0 || m_position.y - DefaultLaserLength > static_cast<float>(GetScreenHeight())) {
        if (m_explodeTime == 0.0f) { m_explodeTime = GetTime(); }
        return GetTime() - m_explodeTime > m_explosionLingerTime;
    }

    return false;
}

void
Laser::UnloadTexture() {
    if (m_explodedtexture.id != 0) {
        ::UnloadTexture(m_explodedtexture);
    }
}

void
Laser::Draw() const {
    if (m_explodeTime != 0.0f) {
        DrawTexture(
            m_explodedtexture,
            m_position.x - m_explodedtexture.width / 2,
            m_position.y <= 0 ? m_position.y : m_position.y - m_explodedtexture.height - DefaultLaserLength, WHITE);
        return;
    }
    DrawRectangle(
        static_cast<int32_t>(m_position.x), // X
        static_cast<int32_t>(m_position.y), // Y
        DefaultLaserWidth, // Width
        DefaultLaserLength, // Height
        Colors::Yellow // Color
    );
}

}
