#include "soundeffect.h"
#include "../engine/engine.h"
#include <cmath>

namespace AbyssEngine {

SoundEffect::SoundEffect(std::unique_ptr<LibAbyss::AudioStream> audioStream) : _audioStream(std::move(audioStream)), _mutex() {
    _audioStream->Stop();

    Engine::Get()->GetSystemIO().AddSoundEffect(this);
}

SoundEffect::~SoundEffect() { Engine::Get()->GetSystemIO().RemoveSoundEffect(this); }

void SoundEffect::Play() { _audioStream->Play(); }

bool SoundEffect::GetIsPlaying() { return _audioStream->IsPlaying(); }

void SoundEffect::Stop() { _audioStream->Stop(); }

void SoundEffect::Pause() { _audioStream->Pause(); }

bool SoundEffect::GetIsPaused() { return _audioStream->IsPaused(); }

bool SoundEffect::GetLoop() { return _audioStream->IsLooped(); }

void SoundEffect::SetLoop(bool looping) { _audioStream->SetLoop(looping); }

float SoundEffect::GetVolume() {
    std::lock_guard<std::mutex> lock(_mutex);

    return _volume;
}

void SoundEffect::SetVolume(float volume) {
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

int16_t SoundEffect::GetSample() {
    std::lock_guard<std::mutex> lock(_mutex);

    auto sample = (int32_t)_audioStream->GetSample();

    if (sample == 0)
        return 0;

    sample = (int32_t)((float)sample * _volumeActual);

    if (sample > 32767)
        sample = 32767;
    else if (sample < -32768)
        sample = -32768;

    return (int16_t)sample;
}

} // namespace AbyssEngine
