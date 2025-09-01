#include <algorithm>

#include "Barrier.h"

#include <cmath>

#include "Colors.h"

namespace SpaceInvaders {

Barrier::Barrier(const Vector2 position) : m_position(position) {
    uint16_t index = 0;
    std::ranges::for_each(BarrierPattern, [&index, this](const char cell) {
        if (cell != '#') { ++index; return; }

        const auto xOff = static_cast<float>(index % BarrierWidth);
        const auto yOff = static_cast<float>(index / BarrierWidth);

        auto cell_ptr = std::make_shared<CellRect>(Vector2 {m_position.x + xOff, m_position.y + yOff});
        m_cellRects.push_back(cell_ptr);
        ++index;
    });
}

void
Barrier::Draw() {
    std::ranges::for_each(m_cellRects, [](auto &cell) {
        if (!cell->GetActive()) { return; }
        cell->Draw();
    });
}

void
Barrier::Damage(const Vector2 pos) const {
    // Convert screen position to relative position within the barrier
    const int relativeX = static_cast<int>(pos.x - m_position.x);
    const int relativeY = static_cast<int>(pos.y - m_position.y);

    // Random damage parameters
    const int damageWidth = GetRandomValue(2, 4);
    const int damageHeight = GetRandomValue(5, 15);
    const int additionalCells = GetRandomValue(8, 15);

    // Create main damage crater
    const int startX = relativeX - damageWidth / 2;
    const int startY = relativeY - damageHeight / 2;

    for (int y = 0; y < damageHeight; ++y) {
        for (int x = 0; x < damageWidth; ++x) {
            const int cellX = startX + x;
            const int cellY = startY + y;

            // Make sure we're within bounds
            if (cellX >= 0 && cellX < BarrierWidth && cellY >= 0 && cellY < BarrierHeight) {
                // Find and damage the cell at this position
                for (auto& cell : m_cellRects) {
                    if (cell->GetActive() &&
                        static_cast<int>(cell->GetPosition().x - m_position.x) == cellX &&
                        static_cast<int>(cell->GetPosition().y - m_position.y) == cellY) {
                        cell->SetActive(false);
                        break;
                    }
                }
            }
        }
    }

    // Damage additional random cells around the impact area
    for (int i = 0; i < additionalCells; ++i) {
        const int randomRadius = GetRandomValue(1, 8);
        const int angle = GetRandomValue(0, 359);

        const int randomX = relativeX + static_cast<int>(randomRadius * cos(angle * DEG2RAD));
        const int randomY = relativeY + static_cast<int>(randomRadius * sin(angle * DEG2RAD));

        // Make sure we're within bounds
        if (randomX >= 0 && randomX < BarrierWidth && randomY >= 0 && randomY < BarrierHeight) {
            // Find and damage the cell at this random position
            for (auto& cell : m_cellRects) {
                if (cell->GetActive() &&
                    static_cast<int>(cell->GetPosition().x - m_position.x) == randomX &&
                    static_cast<int>(cell->GetPosition().y - m_position.y) == randomY) {
                    cell->SetActive(false);
                    break;
                }
            }
        }
    }
}

Rectangle
Barrier::GetRect() {
    return {
        m_position.x,
        m_position.y,
        static_cast<float>(BarrierWidth),
        static_cast<float>(BarrierHeight)};
}


}
