#ifndef ABYSS_SOUNDEFFECT_H
#define ABYSS_SOUNDEFFECT_H

#include <memory>
#include <libabyss/audiostream.h>

namespace AbyssEngine {

class SoundEffect {
    public:
    explicit SoundEffect(std::unique_ptr<LibAbyss::AudioStream> audioStream);
    ~SoundEffect();

    void Play();
    void Stop();
    void Pause();
    float GetVolume();
    void SetVolume(float volume);
    bool GetLoop();
    void SetLoop(bool loop);
    bool GetIsPlaying();
    bool GetIsPaused();
    int16_t GetSample();

  private:
    std::unique_ptr<LibAbyss::AudioStream> _audioStream;
    float _volume = 1.0f;
    float _volumeActual = 1.0f;
    std::mutex _mutex;

};

}

#endif // ABYSS_SOUNDEFFECT_H
