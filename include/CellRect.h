#pragma once

#include <raylib.h>

#include "Laser.h"
#include "Alien.h"

namespace SpaceInvaders {

class CellRect final : public Entity {
public:
    explicit CellRect(const Vector2 &position);
    ~CellRect() override = default;

    void Draw() const override;

    [[nodiscard]] Rectangle GetRect() const override;

private:
};

}