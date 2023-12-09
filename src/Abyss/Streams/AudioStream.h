#pragma once

#include "Abyss/Common/RingBuffer.h"
#include "Abyss/FileSystem/InputStream.h"

#include <ios>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
extern "C" {
#include <libavutil/opt.h>
}

// Compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace Abyss::Streams {

class AudioStream {
    FileSystem::InputStream _stream;
    unsigned char *_avBuffer;
    AVIOContext *_avioContext;
    AVFormatContext *_avFormatContext;
    AVCodecContext *_audioCodecContext;
    SwrContext *_resampleContext;
    AVFrame *_avFrame;
    Common::RingBuffer<uint8_t> _ringBuffer;
    std::mutex _mutex;

    static constexpr int DecodeBufferSize = 1024;

    bool _isPlaying = false;
    bool _isPaused = false;
    bool _loop = false;
    int _audioStreamIdx = 0;
    uint8_t _audioOutBuffer[1024 * 16] = {};

    [[nodiscard]] int streamRead(uint8_t *buffer, int size);
    [[nodiscard]] static int streamReadCallback(void *opaque, uint8_t *buffer, int size);
    [[nodiscard]] static int64_t streamSeekCallback(void *opaque, int64_t offset, int whence);
    [[nodiscard]] int64_t streamSeek(int64_t offset, int whence);
    [[nodiscard]] static std::string AvErrorCodeToString(const int avError);
    void update();

  public:
    explicit AudioStream(FileSystem::InputStream stream);
    ~AudioStream();
    [[nodiscard]] int16_t getSample();
    void setLoop(bool loop);
    [[nodiscard]] bool isLooped();
    [[nodiscard]] bool isPlaying();
    [[nodiscard]] bool isPaused();
    void pause();
    void play();
    void stop();
};

#pragma clang diagnostic pop
} // namespace Abyss::Streams
