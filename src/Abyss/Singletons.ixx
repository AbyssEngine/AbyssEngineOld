module;

#include <stdexcept>

import Abyss.Common.FileProvider;
import Abyss.Common.MouseProvider;
import Abyss.Common.RendererProvider;

export module Abyss.Singletons;

namespace Abyss {

export class Singletons {
    inline static Common::FileProvider *_fileProvider = nullptr;
    inline static Common::RendererProvider *_rendererProvider = nullptr;
    inline static Common::MouseProvider *_mouseProvider = nullptr;

  public:
    [[nodiscard]] static auto getFileProvider() -> Common::FileProvider & {
        if (!_fileProvider)
            throw std::runtime_error("Singletons::getFileProvider: file provider not set");
        return *_fileProvider;
    }

    [[nodiscard]] static auto getRendererProvider() -> Common::RendererProvider & {
        if (!_rendererProvider)
            throw std::runtime_error("Singletons::getRendererProvider: renderer provider not set");
        return *_rendererProvider;
    }

    [[nodiscard]] static auto getMouseProvider() -> Common::MouseProvider & {
        if (!_mouseProvider)
            throw std::runtime_error("Singletons::getMouseProvider: mouse provider not set");
        return *_mouseProvider;
    }

    static auto setFileProvider(Common::FileProvider *abyss_engine) -> void { _fileProvider = abyss_engine; }
    static auto setRendererProvider(Common::RendererProvider *abyss_engine) -> void { _rendererProvider = abyss_engine; }
    static auto setMouseProvider(Common::MouseProvider *abyss_engine) -> void { _mouseProvider = abyss_engine; }
};

} // namespace Abyss
