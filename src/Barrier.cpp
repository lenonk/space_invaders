#include <algorithm>

#include "Barrier.h"
#include "Colors.h"

namespace SpaceInvaders {

Barrier::Barrier(const Vector2 position) : m_position(position) { }

void
Barrier::Draw() const {
    uint16_t index = 0;
    std::ranges::for_each(m_cells, [&index, this](const char cell) {
        if (cell != '#') { ++index; return; }

        const int xOff = index % BarrierWidth;
        const int yOff = index / BarrierWidth;

        DrawRectangle(
            static_cast<int>(m_position.x) + (xOff * CellWidth),
            static_cast<int>(m_position.y) + (yOff * CellHeight),
            CellWidth,
            CellHeight,
            Colors::Yellow
        );
        ++index;
    });
}

}
