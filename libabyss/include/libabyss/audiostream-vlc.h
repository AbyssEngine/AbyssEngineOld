#ifndef ABYSS_AUDIOSTREAM_VLC_H
#define ABYSS_AUDIOSTREAM_VLC_H

#include "inputstream.h"
#include "readerwriterqueue.h"
#include "ringbuffer.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vlcpp/vlc.hpp>

namespace LibAbyss {
class AudioStream {
  public:
    explicit AudioStream(InputStream stream);
    ~AudioStream();
    int16_t GetSample();
    void SetLoop(bool loop);
    bool IsLooped();
    bool IsPlaying();
    bool IsPaused();
    void Pause();
    void Play();
    void Stop();

  private:
    InputStream _stream;
    std::mutex _mutex;

    bool _isPlaying = false;
    bool _isPaused = false;
    bool _loop = false;

    VLC::Instance _instance;
    VLC::Media _media;
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    VLC::MediaList _list;
    VLC::MediaListPlayer _list_player;
#else
    std::thread _looper;
    bool _loop_do = false;
    bool _loop_exit = false;
    std::condition_variable _loop_cv;
#endif
    VLC::MediaPlayer _player;

    moodycamel::ReaderWriterQueue<uint16_t> _queue;
};
} // namespace LibAbyss

#endif // ABYSS_AUDIOSTREAM_VLC_H
