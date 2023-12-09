#include "Singletons.h"

namespace Abyss {

FileSystem::FileLoader &Singletons::getFileProvider() {
    if (!_fileProvider)
        throw std::runtime_error("Singletons::getFileProvider: file provider not set");
    return *_fileProvider;
}

Common::RendererProvider &Singletons::getRendererProvider() {
    if (!_rendererProvider)
        throw std::runtime_error("Singletons::getRendererProvider: renderer provider not set");
    return *_rendererProvider;
}

Common::MouseProvider &Singletons::getMouseProvider() {
    if (!_mouseProvider)
        throw std::runtime_error("Singletons::getMouseProvider: mouse provider not set");
    return *_mouseProvider;
}

Common::SoundEffectProvider &Singletons::getSoundEffectProvider() {
    if (!_rendererProvider)
        throw std::runtime_error("Singletons::getSoundEffectProvider: sound effect provider not set");
    return *_soundEffectProvider;
}

void Singletons::setFileProvider(FileSystem::FileLoader *abyss_engine) { _fileProvider = abyss_engine; }

void Singletons::setRendererProvider(Common::RendererProvider *abyss_engine) { _rendererProvider = abyss_engine; }

void Singletons::setMouseProvider(Common::MouseProvider *abyss_engine) { _mouseProvider = abyss_engine; }

void Singletons::setSoundEffectProvider(Common::SoundEffectProvider *abyss_engine) { _soundEffectProvider = abyss_engine; }

} // namespace Abyss
