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

Rectangle
CellRect::GetRect() const {
    if (GetActive()) {
        return {GetPosition().x, GetPosition().y, 1, 1};
    }

    return {};
}

}
