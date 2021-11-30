#ifndef ABYSS_OVERLOAD_H
#define ABYSS_OVERLOAD_H

namespace AbyssEngine {
template <typename... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

}
#endif // ABYSS_OVERLOAD_H
