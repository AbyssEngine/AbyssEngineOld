#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

#include <variant>
#include <cinttypes>

namespace AbyssEngine {

enum class eMouseButton { Left = 0, Right = 1, Middle = 2 };

struct MouseMoveEvent {
    int X;
    int Y;
};

struct MouseButtonEvent {
    eMouseButton Button;
    bool IsPressed;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseButtonEvent>;

} // namespace AbyssEngine

#endif // ABYSS_EVENTS_H
