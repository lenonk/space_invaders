#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include <raylib.h>

namespace SpaceInvaders {

class Barrier {
public:
    static constexpr uint8_t BarrierHeight = 13;
    static constexpr uint8_t BarrierWidth = 23;

    static constexpr uint8_t CellWidth = 3;
    static constexpr uint8_t CellHeight = 3;

    explicit Barrier(Vector2 position);
    Barrier() = default;
    ~Barrier() = default;

    void Draw() const;

private:
    Vector2 m_position {};
    std::string m_cells {BarrierPattern};

static constexpr std::string_view BarrierPattern =
    "....###############...."
    "...#################..."
    "..###################.."
    ".#####################."
    "#######################"
    "#######################"
    "#######################"
    "#######################"
    "#######################"
    "#######################"
    "######...........######"
    "#####.............#####"
    "####...............####";
};

}
