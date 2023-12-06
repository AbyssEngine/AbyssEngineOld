module;

#include <string_view>

export module Abyss.Common.MouseProvider;

import Abyss.Common.MouseState;

namespace Abyss::Common {

export class MouseProvider {
  public:
    virtual ~MouseProvider() = default;

    virtual auto setCursorImage(std::string_view cursorName) -> void = 0;
    virtual auto setCursorLocked(bool locked) -> void = 0;
    [[nodiscard]] virtual auto getMouseState() -> MouseState & = 0;
};

} // namespace Abyss::Common
