#pragma once

#include "../Common/RingBuffer.h"
#include "InputStream.h"
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

// Compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace Abyss::Streams {

class AudioStream {
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
    Common::RingBuffer<uint8_t> _ringBuffer;
    std::mutex _mutex;

    bool _isPlaying = false;
    bool _isPaused = false;
    bool _loop = false;
    int _audioStreamIdx = 0;
    uint8_t _audioOutBuffer[1024 * 16] = {};

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
};

} // namespace Abyss::Streams
