#ifndef ABYSS_ENUM_H
#define ABYSS_ENUM_H

#define ABYSS_ENUM_BITSET(type)                                                                                                                      \
                                                                                                                                                     \
    inline type operator|(type lhs, type rhs) {                                                                                                      \
        return static_cast<type>(static_cast<std::underlying_type_t<type>>(lhs) | static_cast<std::underlying_type_t<type>>(rhs));                   \
    }                                                                                                                                                \
    inline type operator+(type lhs, type rhs) { return lhs | rhs; }                                                                                  \
    inline void operator+=(type &lhs, type rhs) { lhs = lhs + rhs; }                                                                                 \
                                                                                                                                                     \
    inline type operator-(type lhs, type rhs) {                                                                                                      \
        return static_cast<type>(static_cast<std::underlying_type_t<type>>(lhs) & ~static_cast<std::underlying_type_t<type>>(rhs));                  \
    }                                                                                                                                                \
    inline void operator-=(type &lhs, type rhs) { lhs = lhs - rhs; }                                                                                 \
                                                                                                                                                     \
    inline bool operator&(type lhs, type rhs) {                                                                                                      \
        return (static_cast<std::underlying_type_t<type>>(lhs) & static_cast<std::underlying_type_t<type>>(rhs)) != 0;                               \
    }

#endif // ABYSS_ENUM_H
