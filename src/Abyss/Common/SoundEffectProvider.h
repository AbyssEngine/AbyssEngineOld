#pragma once

#include <cstdint>

namespace Abyss::Common {

class SoundEffectInterface {
  public:
    virtual ~SoundEffectInterface() = default;
    virtual void play() = 0;
    virtual bool getIsPlaying() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual bool getIsPaused() = 0;
    virtual bool getLoop() = 0;
    virtual void setLoop(bool looping) = 0;
    virtual float getVolume() = 0;
    virtual void setVolume(float volume) = 0;
    virtual int16_t getSample() = 0;
};

class SoundEffectProvider {
  public:
    virtual ~SoundEffectProvider() = default;
    virtual void addSoundEffect(SoundEffectInterface *soundEffect) = 0;
    virtual void removeSoundEffect(SoundEffectInterface *soundEffect) = 0;
};

} // namespace Abyss::Common
