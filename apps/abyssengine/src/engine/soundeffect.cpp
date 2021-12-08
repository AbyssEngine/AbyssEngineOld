#include "soundeffect.h"
#include "../engine/engine.h"
#include <cmath>

AbyssEngine::SoundEffect::SoundEffect(std::unique_ptr<LibAbyss::AudioStream> audioStream) : _audioStream(std::move(audioStream)), _mutex() {
    _audioStream->Stop();

    Engine::Get()->GetSystemIO().AddSoundEffect(this);
}

AbyssEngine::SoundEffect::~SoundEffect() { Engine::Get()->GetSystemIO().RemoveSoundEffect(this); }

void AbyssEngine::SoundEffect::Play() { _audioStream->Play(); }

bool AbyssEngine::SoundEffect::GetIsPlaying() { return _audioStream->IsPlaying(); }

void AbyssEngine::SoundEffect::Stop() { _audioStream->Stop(); }

void AbyssEngine::SoundEffect::Pause() { _audioStream->Pause(); }

bool AbyssEngine::SoundEffect::GetIsPaused() { return _audioStream->IsPaused(); }

bool AbyssEngine::SoundEffect::GetLoop() { return _audioStream->IsLooped(); }

void AbyssEngine::SoundEffect::SetLoop(bool looping) { _audioStream->SetLoop(looping); }

float AbyssEngine::SoundEffect::GetVolume() {
    std::lock_guard<std::mutex> lock(_mutex);

    return _volume;
}

void AbyssEngine::SoundEffect::SetVolume(float volume) {
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

int16_t AbyssEngine::SoundEffect::GetSample() {
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
