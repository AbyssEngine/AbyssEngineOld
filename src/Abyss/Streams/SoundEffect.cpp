#include "SoundEffect.h"
#include "Abyss/Singletons.h"
#include <algorithm>
#include <cmath>

namespace Abyss::Streams {

SoundEffect::SoundEffect(std::unique_ptr<AudioStream> audioStream) : _audioStream(std::move(audioStream)), _mutex() {
    _audioStream->stop();

    Singletons::getSoundEffectProvider().addSoundEffect(this);
}

SoundEffect::~SoundEffect() { Singletons::getSoundEffectProvider().removeSoundEffect(this); }

void SoundEffect::play() {
    if (_audioStream->isPlaying())
        _audioStream->stop();
    _audioStream->play();
}

bool SoundEffect::getIsPlaying() { return _audioStream->isPlaying(); }

void SoundEffect::stop() { _audioStream->stop(); }

void SoundEffect::pause() { _audioStream->pause(); }

bool SoundEffect::getIsPaused() { return _audioStream->isPaused(); }

bool SoundEffect::getLoop() { return _audioStream->isLooped(); }

void SoundEffect::setLoop(const bool looping) { _audioStream->setLoop(looping); }

float SoundEffect::getVolume() {
    std::lock_guard lock(_mutex);
    return _volume;
}

void SoundEffect::setVolume(const float volume) {
    std::lock_guard lock(_mutex);

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

int16_t SoundEffect::getSample() {
    std::lock_guard lock(_mutex);

    auto sample = static_cast<int32_t>(_audioStream->getSample());

    if (sample == 0)
        return 0;

    sample = static_cast<int32_t>(static_cast<float>(sample) * _volumeActual);

    sample = std::clamp(sample, -32768, 32767);

    return static_cast<int16_t>(sample);
}

} // namespace Abyss::Streams
