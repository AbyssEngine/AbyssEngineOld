module;

#include <cstdint>

export module Abyss.Common.SoundEffectProvider;

namespace Abyss::Common {

export class SoundEffectInterface {
  public:
    virtual ~SoundEffectInterface() = default;
    virtual auto play() -> void = 0;
    virtual auto getIsPlaying() -> bool = 0;
    virtual auto stop() -> void = 0;
    virtual auto pause() -> void = 0;
    virtual auto getIsPaused() -> bool = 0;
    virtual auto getLoop() -> bool = 0;
    virtual auto setLoop(bool looping) -> void = 0;
    virtual auto getVolume() -> float = 0;
    virtual auto setVolume(float volume) -> void = 0;
    virtual auto getSample() -> int16_t = 0;
};

export class SoundEffectProvider {
  public:
    virtual ~SoundEffectProvider() = default;
    virtual auto addSoundEffect(SoundEffectInterface *soundEffect) -> void = 0;
    virtual auto removeSoundEffect(SoundEffectInterface *soundEffect) -> void = 0;
};

} // namespace Abyss::Common
