#pragma once

#include "Abyss/Common/SoundEffectProvider.h"
#include "AudioStream.h"

#include <memory>
#include <mutex>

namespace Abyss::Streams {

class SoundEffect final : public Common::SoundEffectInterface {
    std::unique_ptr<AudioStream> _audioStream;
    float _volume = 1.0f;
    float _volumeActual = 1.0f;
    std::mutex _mutex;

  public:
    explicit SoundEffect(std::unique_ptr<AudioStream> audioStream);
    ~SoundEffect() override;
    void play() override;
    bool getIsPlaying() override;
    void stop() override;
    void pause() override;
    bool getIsPaused() override;
    bool getLoop() override;
    void setLoop(bool looping) override;
    float getVolume() override;
    void setVolume(float volume) override;
    int16_t getSample() override;
};

} // namespace Abyss::Streams
