#include "libabyss/audiostream.h"
#include <absl/cleanup/cleanup.h>
#include <ios>
#include <mutex>

LibAbyss::AudioStream::AudioStream(InputStream stream)
    : _stream(std::move(stream))
#if LIBVLC_VERSION_INT < LIBVLC_VERSION(4, 0, 0, 0)
      ,
      _looper([this]() {
          // libvlc doesn't currently provide a way to loop the playback, hence this hack
          std::unique_lock<std::mutex> lk(_mutex);
          while (true) {
              _loop_cv.wait(lk, [this]() { return _loop_do || _loop_exit; });
              if (_loop_exit)
                  break;
              if (_loop_do) {
                  _loop_do = false;
                  _player.stop();
                  _player.play();
              }
          }
      })
#endif
{
    const char *const vlc_args[] = {
        "--ignore-config", "--no-lua",
        //"--verbose=2",
    };
    _instance = VLC::Instance(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

    _media = VLC::Media(
        _instance,
        [this](void *, void **, uint64_t *p_size) -> int {
            _stream.clear();
            *p_size = _stream.size();
            _stream.seekg(0, std::ios_base::beg);
            return 0;
        },
        [this](void *, unsigned char *buf, size_t size) -> ssize_t {
            _stream.read((char *)buf, size);
            return _stream.gcount();
        },
        [this](void *, uint64_t seek) -> int {
            _stream.seekg(seek, std::ios_base::beg);
            return 0;
        },
        [](void *) {});

#if LIBVLC_VERSION_INT < LIBVLC_VERSION(4, 0, 0, 0)
    _player = VLC::MediaPlayer(_media);
    _player.eventManager().onEndReached([this]() {
        std::lock_guard lock(_mutex);
        if (_loop) {
            _loop_do = true;
            _loop_cv.notify_all();
        } else {
            _isPlaying = false;
        }
    });
#else
    _player = VLC::MediaPlayer(_instance);
#endif

    _player.setAudioFormat("S16N", 44100, /* channels */ 2);
    _player.setAudioCallbacks(
        [this](const void *samples, unsigned int count, int64_t pts) {
            if (_queue.size_approx() > count * 10) {
                return;
            }
            const uint8_t *data = reinterpret_cast<const uint8_t *>(samples);
            for (unsigned int i = 0; i < count * /* channels */ 2; ++i) {
                uint16_t value = (data[i * 2 + 1] << 8) | data[i * 2];
                _queue.enqueue(value);
            }
        },
        nullptr, nullptr, nullptr, nullptr);

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    _list.addMedia(_media);
    _list_player = VLC::MediaListPlayer(_instance);
    _list_player.setMediaPlayer(_player);
    _list_player.setMediaList(_list);
#endif
}

LibAbyss::AudioStream::~AudioStream() {
#if LIBVLC_VERSION_INT < LIBVLC_VERSION(4, 0, 0, 0)
    {
        std::lock_guard lock(_mutex);
        _loop_exit = true;
        _loop_cv.notify_all();
    }
    _looper.join();
#endif
}

int16_t LibAbyss::AudioStream::GetSample() {
    int16_t result = 0;
    _queue.try_dequeue(result);
    return result;
}

void LibAbyss::AudioStream::SetLoop(bool loop) {
    std::lock_guard<std::mutex> lock(_mutex);

    _loop = loop;
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    _list_player.setPlaybackMode(loop ? libvlc_playback_mode_loop : libvlc_playback_mode_default);
#endif
}

bool LibAbyss::AudioStream::IsLooped() {
    std::lock_guard<std::mutex> lock(_mutex);

    return _loop;
}

bool LibAbyss::AudioStream::IsPlaying() {
    std::lock_guard<std::mutex> lock(_mutex);

    return _isPlaying;
}

bool LibAbyss::AudioStream::IsPaused() {
    std::lock_guard<std::mutex> lock(_mutex);

    return _isPaused;
}

void LibAbyss::AudioStream::Pause() {
    std::lock_guard<std::mutex> lock(_mutex);

    _player.setPause(true);
    _isPaused = true;
}

void LibAbyss::AudioStream::Play() {
    std::lock_guard<std::mutex> lock(_mutex);
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    _list_player.play();
#else
    _player.stop();
    _player.play();
#endif

    _isPaused = false;
    _isPlaying = true;
    return;
}

void LibAbyss::AudioStream::Stop() {
    std::lock_guard<std::mutex> lock(_mutex);
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    _list_player.stopAsync();
#else
    _player.stop();
#endif

    _isPlaying = false;
    _isPaused = false;
}
