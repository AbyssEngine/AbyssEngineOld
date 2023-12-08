module;

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

export module Abyss.Streams.AudioStream;

import Abyss.FileSystem.InputStream;
import Abyss.Common.RingBuffer;

namespace Abyss::Streams {

export class AudioStream {
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

    [[nodiscard]] auto streamRead(uint8_t *buffer, const int size) -> int {
        _stream.read(reinterpret_cast<char *>(buffer), size);
        if (_stream) {
            return static_cast<int>(_stream.gcount());
        }
        return -1;
    }

    [[nodiscard]] static auto streamReadCallback(void *opaque, uint8_t *buffer, const int size) -> int {
        return static_cast<AudioStream *>(opaque)->streamRead(buffer, size);
    }

    [[nodiscard]] static auto streamSeekCallback(void *opaque, const int64_t offset, const int whence) -> int64_t {
        return static_cast<AudioStream *>(opaque)->streamSeek(offset, whence);
    }

    [[nodiscard]] auto streamSeek(const int64_t offset, const int whence) -> int64_t {
        std::ios_base::seekdir dir;
        _stream.clear();

        switch (whence) {
        case SEEK_SET:
            dir = std::ios_base::beg;
            break;
        case SEEK_CUR:
            dir = std::ios_base::cur;
            break;
        case SEEK_END:
            dir = std::ios_base::end;
            break;
        case AVSEEK_SIZE:
            return _stream.size();
        default:
            return -1;
        }

        _stream.seekg(offset, dir);
        return _stream.tellg();
    }

    [[nodiscard]] static auto AvErrorCodeToString(const int avError) -> std::string {
        char str[2048] = {};
        av_make_error_string(str, 2048, avError);
        return {std::string(str)};
    }

    auto update() -> void {
        if (_avFormatContext == nullptr)
            return;

        int avError;
        const std::unique_ptr<AVPacket, void (*)(AVPacket *)> packet(new AVPacket(), [](AVPacket *p) {
            av_packet_free(&p);
            delete p;
        });

        if ((avError = av_read_frame(_avFormatContext, packet.get())) < 0) {
            if (_loop) {
                av_seek_frame(_avFormatContext, _audioStreamIdx, 0, AVSEEK_FLAG_FRAME);
                return;
            }

            av_seek_frame(_avFormatContext, _audioStreamIdx, 0, AVSEEK_FLAG_FRAME);
            _isPlaying = false;
            return;
        }

        if (packet->stream_index != _audioStreamIdx)
            return;

        if ((avError = avcodec_send_packet(_audioCodecContext, packet.get())) < 0) {
            avcodec_flush_buffers(_audioCodecContext);
            avformat_flush(_avFormatContext);
            av_seek_frame(_avFormatContext, _audioStreamIdx, 0, AVSEEK_FLAG_FRAME);

            if (!_loop)
                _isPlaying = false;

            return;
        }

        while (true) {
            if ((avError = avcodec_receive_frame(_audioCodecContext, _avFrame)) < 0) {
                if (avError == AVERROR(EAGAIN) || avError == AVERROR_EOF)
                    return;

                throw std::runtime_error("Failed to receive frame: " + AvErrorCodeToString(avError));
            }

            int _lineSize;
            const auto outSamples = swr_get_out_samples(_resampleContext, _avFrame->nb_samples);
            const auto audioOutSize = av_samples_get_buffer_size(&_lineSize, 2, outSamples, AV_SAMPLE_FMT_S16, 0);
            uint8_t *ptr[1] = {_audioOutBuffer};
            const auto result = swr_convert(_resampleContext, ptr, audioOutSize, const_cast<const uint8_t **>(_avFrame->data), _avFrame->nb_samples);
            _ringBuffer.pushData(std::span(_audioOutBuffer, result * 4));
        }
    }

  public:
    explicit AudioStream(FileSystem::InputStream stream) : _stream(std::move(stream)), _ringBuffer(1024 * 1024) {
        const auto streamSize = _stream.size();
        const int decodeBufferSize = streamSize < DecodeBufferSize ? static_cast<int>(streamSize) : DecodeBufferSize;

        _avBuffer = static_cast<unsigned char *>(av_malloc(DecodeBufferSize)); // AVIO is going to free this automagically... because why not?
        memset(_avBuffer, 0, DecodeBufferSize);

        _avioContext = avio_alloc_context(_avBuffer, decodeBufferSize, 0, this, &AudioStream::streamReadCallback, nullptr, &AudioStream::streamSeekCallback);

        _avFormatContext = avformat_alloc_context();
        _avFormatContext->pb = _avioContext;
        _avFormatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

        int avError;

        if ((avError = avformat_open_input(&_avFormatContext, "", nullptr, nullptr)) < 0)
            throw std::runtime_error("Failed to open input stream: " + AvErrorCodeToString(avError));

        if ((avError = avformat_find_stream_info(_avFormatContext, nullptr)) < 0)
            throw std::runtime_error("Failed to find stream info: " + AvErrorCodeToString(avError));

        _audioStreamIdx = -1;
        for (uint32_t i = 0; i < _avFormatContext->nb_streams; i++) {
            if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
                continue;

            _audioStreamIdx = static_cast<int>(i);
            break;
        }

        if (_audioStreamIdx < 0)
            throw std::runtime_error("No audio stream found");

        const auto audioCodecPar = _avFormatContext->streams[_audioStreamIdx]->codecpar;
        auto audioDecoder = avcodec_find_decoder(audioCodecPar->codec_id);

        if (audioDecoder == nullptr)
            throw std::runtime_error("Missing audio codec.");

        _audioCodecContext = avcodec_alloc_context3(audioDecoder);
        if ((avError = avcodec_parameters_to_context(_audioCodecContext, audioCodecPar)) < 0)
            throw std::runtime_error("Failed to copy codec parameters to decoder context: " + AvErrorCodeToString(avError));

        if ((avError = avcodec_open2(_audioCodecContext, audioDecoder, nullptr)) < 0)
            throw std::runtime_error("Failed to open audio codec: " + AvErrorCodeToString(avError));

        _resampleContext = swr_alloc();

        av_opt_set_channel_layout(_resampleContext, "in_channel_layout", av_get_default_channel_layout(_audioCodecContext->channels), 0);
        av_opt_set_int(_resampleContext, "in_sample_rate", _audioCodecContext->sample_rate, 0);
        av_opt_set_sample_fmt(_resampleContext, "in_sample_fmt", _audioCodecContext->sample_fmt, 0);

        av_opt_set_channel_layout(_resampleContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(_resampleContext, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(_resampleContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

        if ((avError = swr_init(_resampleContext)) < 0)
            throw std::runtime_error("Failed to initialize resampler: " + AvErrorCodeToString(avError));

        _avFrame = av_frame_alloc();
    }

    ~AudioStream() {
        av_free(_avioContext->buffer);
        avio_context_free(&_avioContext);
        if (_audioStreamIdx >= 0) {
            avcodec_free_context(&_audioCodecContext);
            swr_free(&_resampleContext);
        }
        av_frame_free(&_avFrame);
        avformat_close_input(&_avFormatContext);
        avformat_free_context(_avFormatContext);
    }

    [[nodiscard]] auto getSample() -> int16_t {
        std::lock_guard lock(_mutex);

        if (_isPaused || (!_isPlaying && _ringBuffer.available() == 0))
            return 0;

        if (_ringBuffer.available() == 0 && _isPlaying)
            update();

        if (!_isPlaying && _ringBuffer.available() == 0)
            return 0;

        uint8_t data[2];
        _ringBuffer.readData(std::span(data, 2));
        return static_cast<int16_t>(static_cast<uint16_t>(data[0] & 0xFF) | static_cast<uint16_t>(data[1]) << 8);
    }

    auto setLoop(const bool loop) -> void {
        std::lock_guard lock(_mutex);

        _loop = loop;
    }

    [[nodiscard]] auto isLooped() -> bool {
        std::lock_guard lock(_mutex);

        return _loop;
    }

    [[nodiscard]] auto isPlaying() -> bool {
        std::lock_guard lock(_mutex);

        return _isPlaying;
    }

    [[nodiscard]] auto isPaused() -> bool {
        std::lock_guard lock(_mutex);

        return _isPaused;
    }

    auto pause() -> void {
        std::lock_guard lock(_mutex);

        _isPaused = true;
    }

    auto play() -> void {
        std::lock_guard lock(_mutex);

        _isPaused = false;
        _isPlaying = true;
    }

    auto stop() -> void {
        std::lock_guard lock(_mutex);

        _isPlaying = false;
        _isPaused = false;
    }
};

#pragma clang diagnostic pop
} // namespace Abyss::Streams
