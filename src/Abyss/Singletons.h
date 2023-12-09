#pragma once

#include "Common/MouseProvider.h"
#include "Common/RendererProvider.h"
#include "Common/SoundEffectProvider.h"
#include "FileSystem/FileLoader.h"

#include <stdexcept>

namespace Abyss {

class Singletons {
    inline static FileSystem::FileLoader *_fileProvider = nullptr;
    inline static Common::RendererProvider *_rendererProvider = nullptr;
    inline static Common::MouseProvider *_mouseProvider = nullptr;
    inline static Common::SoundEffectProvider *_soundEffectProvider = nullptr;

  public:
    [[nodiscard]] static FileSystem::FileLoader &getFileProvider();
    [[nodiscard]] static Common::RendererProvider &getRendererProvider();
    [[nodiscard]] static Common::MouseProvider &getMouseProvider();
    [[nodiscard]] static Common::SoundEffectProvider &getSoundEffectProvider();

    static void setFileProvider(FileSystem::FileLoader *abyss_engine);
    static void setRendererProvider(Common::RendererProvider *abyss_engine);
    static void setMouseProvider(Common::MouseProvider *abyss_engine);
    static void setSoundEffectProvider(Common::SoundEffectProvider *abyss_engine);
};

} // namespace Abyss
