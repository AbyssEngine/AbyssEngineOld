#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

#include <cinttypes>
#include <type_traits>
#include <variant>

namespace AbyssEngine {

enum class eMouseButton : uint8_t { Left = 1, Right = 2, Middle = 4 };

inline eMouseButton operator|(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

inline eMouseButton operator&(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) & static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

inline eMouseButton operator-(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) &
                                     ~static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

inline bool operator==(eMouseButton lhs, eMouseButton rhs) {
    return (static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs)) > 0;
}

inline bool operator!=(eMouseButton lhs, eMouseButton rhs) {
    return (static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs)) == 0;
}

inline void operator-=(eMouseButton &lhs, eMouseButton rhs) {
    lhs = static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) & ~static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

inline eMouseButton operator+(eMouseButton lhs, eMouseButton rhs) {
    return static_cast<eMouseButton>(static_cast<std::underlying_type_t<eMouseButton>>(lhs) | static_cast<std::underlying_type_t<eMouseButton>>(rhs));
}

inline void operator+=(eMouseButton &lhs, eMouseButton rhs) {
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
