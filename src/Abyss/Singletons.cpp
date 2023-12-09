#include "Singletons.h"

namespace Abyss {

namespace {

FileSystem::FileLoader *gFileProvider = nullptr;
Common::RendererProvider *gRendererProvider = nullptr;
Common::MouseProvider *gMouseProvider = nullptr;
Common::SoundEffectProvider *gSoundEffectProvider = nullptr;

} // namespace


FileSystem::FileLoader &Singletons::getFileProvider() {
    if (!gFileProvider)
        throw std::runtime_error("Singletons::getFileProvider: file provider not set");
    return *gFileProvider;
}

Common::RendererProvider &Singletons::getRendererProvider() {
    if (!gRendererProvider)
        throw std::runtime_error("Singletons::getRendererProvider: renderer provider not set");
    return *gRendererProvider;
}

Common::MouseProvider &Singletons::getMouseProvider() {
    if (!gMouseProvider)
        throw std::runtime_error("Singletons::getMouseProvider: mouse provider not set");
    return *gMouseProvider;
}

Common::SoundEffectProvider &Singletons::getSoundEffectProvider() {
    if (!gSoundEffectProvider)
        throw std::runtime_error("Singletons::getSoundEffectProvider: sound effect provider not set");
    return *gSoundEffectProvider;
}

void Singletons::setFileProvider(FileSystem::FileLoader *abyss_engine) { gFileProvider = abyss_engine; }

void Singletons::setRendererProvider(Common::RendererProvider *abyss_engine) { gRendererProvider = abyss_engine; }

void Singletons::setMouseProvider(Common::MouseProvider *abyss_engine) { gMouseProvider = abyss_engine; }

void Singletons::setSoundEffectProvider(Common::SoundEffectProvider *abyss_engine) { gSoundEffectProvider = abyss_engine; }

} // namespace Abyss
