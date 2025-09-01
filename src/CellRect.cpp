#include "CellRect.h"

#include "Colors.h"

namespace SpaceInvaders {
CellRect::CellRect(const Vector2 &position) {
    m_position = position;
}

void
CellRect::Draw() {
    DrawRectangleV(m_position, {1, 1}, Colors::Yellow);
}

bool
CellRect::GetActive() const {
    return m_active;
}

void
CellRect::SetActive(const bool active) {
    m_active = active;
}

Rectangle
CellRect::GetRect() {
    if (m_active) {
        return {m_position.x, m_position.y, 1, 1};
    }

    return {};
}

}
