#pragma once

#include "MouseState.h"

#include <string_view>

namespace Abyss::Common {

class MouseProvider {
  public:
    virtual ~MouseProvider() = default;

    virtual auto setCursorImage(std::string_view cursorName) -> void = 0;
    virtual auto setCursorLocked(bool locked) -> void = 0;
    [[nodiscard]] virtual auto getMouseState() -> MouseState & = 0;
};

} // namespace Abyss::Common
