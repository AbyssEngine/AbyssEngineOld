#ifndef ABYSS_AUDIOSTREAM_H
#define ABYSS_AUDIOSTREAM_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};

#include "inputstream.h"
#include "ringbuffer.h"
#include <memory>
#include <mutex>

// Compatability with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

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
    int StreamRead(uint8_t *buffer, int size);
    static int StreamReadCallback(void *opaque, uint8_t *buffer, int size) { return ((AudioStream *)opaque)->StreamRead(buffer, size); };
    int64_t StreamSeek(int64_t offset, int whence);
    static int64_t StreamSeekCallback(void *opaque, int64_t offset, int whence) { return ((AudioStream *)opaque)->StreamSeek(offset, whence); };
    static std::string AvErrorCodeToString(int avError);
    void Update();

    InputStream _stream;
    unsigned char *_avBuffer;
    AVIOContext *_avioContext;
    AVFormatContext *_avFormatContext;
    AVCodecContext *_audioCodecContext;
    SwrContext *_resampleContext;
    AVFrame *_avFrame;
    RingBuffer _ringBuffer;
    uint8_t **_destData;
    int _lineSize;

    std::mutex _mutex;

    bool _isPlaying = false;
    bool _isPaused = false;
    bool _loop = false;
    int _audioStreamIdx = 0;
};
} // namespace LibAbyss

#endif // ABYSS_AUDIOSTREAM_H
