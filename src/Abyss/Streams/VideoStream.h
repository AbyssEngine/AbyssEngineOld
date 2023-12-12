#pragma once

#include "Abyss/FileSystem/InputStream.h"
#include "AudioStream.h"

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

// Compatability with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace Abyss::Streams {

constexpr int DecodeBufferSize = 1024;

class VideoStream {
    FileSystem::InputStream _stream;
    std::unique_ptr<AudioStream> _separateAudio;
    Common::RingBuffer<uint8_t> _ringBuffer;

    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> _texture;

    AVFormatContext *_avFormatContext;
    AVIOContext *_avioContext;
    SwrContext *_resampleContext;
    AVCodecContext *_videoCodecContext;
    AVCodecContext *_audioCodecContext;
    AVFrame *_avFrame;

    unsigned char *_avBuffer;
    std::vector<uint8_t> _yPlane;
    std::vector<uint8_t> _uPlane;
    std::vector<uint8_t> _vPlane;
    SwsContext *_swsContext;

    SDL_Rect _sourceRect;
    SDL_Rect _targetRect;

    int _videoStreamIdx = -1;
    int _audioStreamIdx = -1;
    int _uvPitch;
    uint64_t _microsPerFrame;
    uint64_t _videoTimestamp;
    bool _isPlaying = true;
    bool _framesReady = false;
    uint32_t _totalTicks = 0;
    uint8_t _audioOutBuffer[1024 * 16] = {};

    static std::string avErrorCodeToString(int avError);
    int videoStreamRead(uint8_t *buffer, int size);
    bool processFrame();
    int64_t videoStreamSeek(int64_t offset, int whence);

  public:
    VideoStream(Abyss::FileSystem::InputStream stream, std::optional<Abyss::FileSystem::InputStream> separateAudio);
    ~VideoStream();
    void update(uint32_t ticks);
    void render() const;
    void stopVideo();
    short getAudioSample();
    bool getIsPlaying() const;
};

} // namespace Abyss::Streams
