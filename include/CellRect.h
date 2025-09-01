#pragma once

#include <raylib.h>

#include "Laser.h"
#include "Alien.h"

namespace SpaceInvaders {

class CellRect final : public Entity {
public:
    explicit CellRect(const Vector2 &position);
    ~CellRect() override = default;

    void Draw() override;

    [[nodiscard]] bool GetActive() const;
    [[nodiscard]] Rectangle GetRect() override;

    void SetActive(const bool active);

private:
    bool m_active {true};
};

}