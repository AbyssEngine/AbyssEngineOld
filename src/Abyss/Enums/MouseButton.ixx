module;

#include <cstdint>

export module Abyss.Enums.MouseButton;

namespace Abyss::Enums {

export enum class MouseButton : uint8_t {
    Left = 0x01,
    Right = 0x02,
    Middle = 0x04,
};

} // namespace Abyss::Enums
