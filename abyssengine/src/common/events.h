#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

#include <cinttypes>
#include <type_traits>
#include <variant>

namespace AbyssEngine {

enum class eMouseButton : uint8_t { Left = 1, Right = 2, Middle = 4 };

static eMouseButton operator|(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

static eMouseButton operator&(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) & static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

static eMouseButton operator-(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) &
                                     ~static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

static bool operator==(eMouseButton lhs, eMouseButton rhs) {
    return (static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs)) > 0;
}

static bool operator!=(eMouseButton lhs, eMouseButton rhs) {
    return (static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs)) == 0;
}

static void operator-=(eMouseButton &lhs, eMouseButton rhs) {
    lhs = static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) & ~static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

static eMouseButton operator+(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

static void operator+=(eMouseButton &lhs, eMouseButton rhs) {
    lhs = static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

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
