#include <algorithm>
#include <cmath>

#include "Barrier.h"

#include "Logger.h"

namespace SpaceInvaders {

Barrier::Barrier(const Vector2 position) : m_position(position) {
    uint16_t index = 0;
    std::ranges::for_each(BarrierPattern, [&index, this](const char cell) {
        if (cell != '#') { ++index; return; }

        const auto xOff = static_cast<float>(index % BarrierWidth);
        const auto yOff = static_cast<float>(index / BarrierWidth);

        const auto cell_ptr = std::make_shared<CellRect>(Vector2 {m_position.x + xOff, m_position.y + yOff});
        m_cellRects.push_back(cell_ptr);
        ++index;
    });
}

Barrier::~Barrier() {
    m_cellRects.clear();
}

void
Barrier::Draw() const {
    std::ranges::for_each(m_cellRects, [](auto &cell) {
        if (!cell->GetActive()) { return; }
        cell->Draw();
    });
}

void
Barrier::Damage(const PlayerLaser &laser) const {
    Damage(laser.GetPosition(), -1);
}

void
Barrier::Damage(const AlienLaser &laser) const {
    Damage(laser.GetPosition(), 1);
}

// There are several techniques to make this function more efficient...use a dense array instead of a sparse array
// and calculate index, use a map instead of a vector, etc...but I can't be arsed...
void
Barrier::Damage(const Vector2 pos, const int8_t direction) const {
    // Impact position relative to barrier grid
    const int32_t impactX = std::clamp(static_cast<int32_t>(std::lround(pos.x - m_position.x)), 0, static_cast<int32_t>(BarrierWidth) - 1);
    const auto impactY = static_cast<int32_t>(std::lround(pos.y - m_position.y));

    if (impactY < -10 || impactY > BarrierHeight + 10) { return; } // Ignore out of bounds

    // DEBUG: Print impact coordinates for alien lasers
    // LogDebug(std::format("Alien laser impact: screen({:.2f}, {:.2f}) -> barrier({}, {}) | barrier_pos({:.2f}, {:.2f})\n",
    //     pos.x, pos.y, impactX, impactY, m_position.x, m_position.y));

    // Always destroy the directly hit cell first
    for (auto &cell : m_cellRects) {
        if (static_cast<int16_t>(cell->GetPosition().x - m_position.x) == impactX &&
            static_cast<int16_t>(cell->GetPosition().y - m_position.y) == impactY) {
            cell->SetActive(false);
            break;
        }
    }

    // Damage approximately 500 random pixels around impact, with probability decreasing by distance
    constexpr int16_t targetDamage = 500;
    constexpr int16_t maxRadius = 15; // Maximum search radius

    int16_t destroyed = 0;
    for (int16_t attempt = 0; destroyed < destroyed * 0.90f || attempt < targetDamage * 15; ++attempt) {
        // Generate random offset from impact point
        const int dx = GetRandomValue(-maxRadius, maxRadius);
        const int dy = GetRandomValue(-maxRadius, maxRadius);

        const int16_t targetX = impactX + dx;
        const int16_t targetY = impactY + dy;

        // Skip if out of bounds
        if (targetX < 0 || targetX >= BarrierWidth || targetY < 0 || targetY >= BarrierHeight)
            continue;

        // Calculate distance from impact point
        const float distance = std::sqrt(static_cast<float>(dx * dx + dy * dy));

        // Calculate directional bias: cells in the laser's direction are more likely to be hit
        const float directionalWeight = (dy * direction > 0) ? 1.5f : 1.0f; // 1.5x weight for correct direction

        // Probability decreases with distance, increases with directional alignment
        // Close cells (distance < 3) have ~90% chance, far cells (distance > 10) have ~10% chance
        const float baseProbability = std::max(0.1f, 1.0f - (distance / 15.0f));
        const float adjustedProbability = std::min(0.95f, baseProbability * directionalWeight);

        // Random chance based on calculated probability
        const auto chance = static_cast<int32_t>(adjustedProbability * 100);
        if (GetRandomValue(1, 100) > chance) continue;

        // Find and damage the cell at this position
        for (auto& cell : m_cellRects) {
            if (cell->GetActive() &&
                static_cast<int16_t>(cell->GetPosition().x - m_position.x) == targetX &&
                static_cast<int16_t>(cell->GetPosition().y - m_position.y) == targetY) {
                destroyed++;
                cell->SetActive(false);
                break;
            }
        }
    }
}

Rectangle
Barrier::GetRect() const {
   return {
        m_position.x,
        m_position.y,
        static_cast<float>(BarrierWidth),
        static_cast<float>(BarrierHeight)
    };
}

}
