#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

#include <cinttypes>
#include <type_traits>
#include <variant>
#include "enum.h"

namespace AbyssEngine {

enum class eMouseButton : uint8_t { Left = 1, Right = 2, Middle = 4 };
ABYSS_ENUM_BITSET(eMouseButton);

struct MouseMoveEvent {
    int X;
    int Y;
};

struct MouseButtonEvent {
    eMouseButton Button;
    bool IsPressed;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseButtonEvent>;

struct KeyboardEvent {
    uint16_t Scancode;
    bool Pressed;
};

} // namespace AbyssEngine

#endif // ABYSS_EVENTS_H
