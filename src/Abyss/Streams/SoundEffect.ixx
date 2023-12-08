module;

#include <algorithm>
#include <cmath>
#include <memory>
#include <mutex>

import Abyss.Enums.MouseButton;

export module Abyss.Streams.SoundEffect;

import Abyss.Streams.AudioStream;
import Abyss.Singletons;
import Abyss.Common.SoundEffectProvider;

namespace Abyss::Streams {

export class SoundEffect : public Abyss::Common::SoundEffectInterface {
    std::unique_ptr<Abyss::Streams::AudioStream> _audioStream;
    float _volume = 1.0f;
    float _volumeActual = 1.0f;
    std::mutex _mutex;

  public:
    explicit SoundEffect(std::unique_ptr<Abyss::Streams::AudioStream> audioStream) : _audioStream(std::move(audioStream)), _mutex() {
        _audioStream->stop();

        Singletons::getSoundEffectProvider().addSoundEffect(this);
    }

    ~SoundEffect() { Singletons::getSoundEffectProvider().removeSoundEffect(this); }

    auto play() -> void {
        if (_audioStream->isPlaying())
            _audioStream->stop();
        _audioStream->play();
    }

    auto getIsPlaying() -> bool { return _audioStream->isPlaying(); }

    auto stop() -> void { _audioStream->stop(); }

    auto pause() -> void { _audioStream->pause(); }

    auto getIsPaused() -> bool { return _audioStream->isPaused(); }

    auto getLoop() -> bool { return _audioStream->isLooped(); }

    auto setLoop(bool looping) -> void { _audioStream->setLoop(looping); }

    auto getVolume() -> float {
        std::lock_guard<std::mutex> lock(_mutex);

        return _volume;
    }

    auto setVolume(float volume) -> void {
        std::lock_guard<std::mutex> lock(_mutex);

        _volume = volume;
        _volumeActual = std::pow(volume, 2.0f);

        if (_volume > 1.0f) {
            _volume = 1.0f;
        } else if (_volume < 0.0f) {
            _volume = 0.0f;
        }

        if (_volumeActual > 1.0f) {
            _volumeActual = 1.0f;
        } else if (_volumeActual < 0.0f) {
            _volumeActual = 0.0f;
        }
    }

    auto getSample() -> int16_t {
        std::lock_guard<std::mutex> lock(_mutex);

        auto sample = (int32_t)_audioStream->getSample();

        if (sample == 0)
            return 0;

        sample = (int32_t)((float)sample * _volumeActual);

        sample = std::clamp(sample, -32768, 32767);

        return (int16_t)sample;
    }
};

} // namespace Abyss::Streams
