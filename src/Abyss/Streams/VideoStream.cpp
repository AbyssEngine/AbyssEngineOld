#include "VideoStream.h"

#include "Abyss/Singletons.h"
#include <absl/strings/str_cat.h>

namespace Abyss::Streams {

std::string VideoStream::avErrorCodeToString(const int avError) {
    char str[2048] = {};

    av_make_error_string(str, 2048, avError);

    return {std::string(str)};
}

int VideoStream::videoStreamRead(uint8_t *buffer, int size) {
    if (!_isPlaying)
        return 0;

    _stream.read(reinterpret_cast<char *>(buffer), size);
    if (_stream)
        return static_cast<int>(_stream.gcount());

    return -1;
}

bool VideoStream::processFrame() {
    if (_avFormatContext == nullptr || !_isPlaying)
        return false;

    const std::unique_ptr<AVPacket, void (*)(AVPacket *)> packet(av_packet_alloc(), [](AVPacket *p) { av_packet_free(&p); });

    if (av_read_frame(_avFormatContext, packet.get()) < 0) {
        _isPlaying = false;
        return true;
    }

    if (packet->stream_index == _videoStreamIdx) {
        int avError;

        if ((avError = avcodec_send_packet(_videoCodecContext, packet.get())) < 0)
            throw std::runtime_error(absl::StrCat("Error decoding video packet: ", avErrorCodeToString(avError)));

        if ((avError = avcodec_receive_frame(_videoCodecContext, _avFrame)) < 0)
            throw std::runtime_error(absl::StrCat("Error decoding video packet: ", avErrorCodeToString(avError)));

        uint8_t *data[AV_NUM_DATA_POINTERS];
        data[0] = _yPlane.data();
        data[1] = _uPlane.data();
        data[2] = _vPlane.data();

        int lineSize[AV_NUM_DATA_POINTERS];
        lineSize[0] = _videoCodecContext->width;
        lineSize[1] = _uvPitch;
        lineSize[2] = _uvPitch;

        _framesReady = true;

        sws_scale(_swsContext, _avFrame->data, _avFrame->linesize, 0, _videoCodecContext->height, data, lineSize);
        if (SDL_UpdateYUVTexture(_texture.get(), nullptr, _yPlane.data(), _videoCodecContext->width, _uPlane.data(), _uvPitch, _vPlane.data(), _uvPitch) < 0) {
            throw std::runtime_error("Cannot set YUV data");
        }

        return true;
    }

    if (packet->stream_index == _audioStreamIdx) {
        int avError;

        if ((avError = avcodec_send_packet(_audioCodecContext, packet.get())) < 0)
            throw std::runtime_error(absl::StrCat("Error decoding audio packet: ", avErrorCodeToString(avError)));

        while (true) {
            if ((avError = avcodec_receive_frame(_audioCodecContext, _avFrame)) < 0) {
                if (avError == AVERROR(EAGAIN) || avError == AVERROR_EOF)
                    break;

                throw std::runtime_error(absl::StrCat("Error decoding audio packet: ", avErrorCodeToString(avError)));
            }

            int _lineSize;
            const auto outSamples = swr_get_out_samples(_resampleContext, _avFrame->nb_samples);
            const auto audioOutSize = av_samples_get_buffer_size(&_lineSize, 2, outSamples, AV_SAMPLE_FMT_S16, 0);
            uint8_t *ptr[1] = {_audioOutBuffer};
            const auto result = swr_convert(_resampleContext, ptr, audioOutSize, const_cast<const uint8_t **>(_avFrame->data), _avFrame->nb_samples);
            _ringBuffer.pushData(std::span(_audioOutBuffer, result * 4));
        }

        return false;
    }

    return false;
}

int64_t VideoStream::videoStreamSeek(const int64_t offset, const int whence) {
    if (!_isPlaying)
        return -1;
    _stream.clear();

    std::ios_base::seekdir dir;

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

VideoStream::VideoStream(FileSystem::InputStream stream, std::optional<FileSystem::InputStream> separateAudio)
    : _stream(std::move(stream)), _ringBuffer(1024 * 4096), _texture(nullptr, SDL_DestroyTexture), _videoCodecContext(), _audioCodecContext(), _avFrame(),
      _avBuffer(), _swsContext(), _sourceRect(), _targetRect(), _microsPerFrame(0), _videoTimestamp(0) {
    _avBuffer = static_cast<unsigned char *>(av_malloc(DecodeBufferSize)); // AVIO is going to free this automagically... because why not?
    memset(_avBuffer, 0, DecodeBufferSize);

    _avioContext = avio_alloc_context(
        _avBuffer, DecodeBufferSize, 0, this,
        [](void *opaque, uint8_t *buffer, const int size) { return static_cast<VideoStream *>(opaque)->videoStreamRead(buffer, size); }, nullptr,
        [](void *opaque, const int64_t offset, const int whence) { return static_cast<VideoStream *>(opaque)->videoStreamSeek(offset, whence); });

    _avFormatContext = avformat_alloc_context();
    _avFormatContext->pb = _avioContext;
    _avFormatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

    int avError;
    if ((avError = avformat_open_input(&_avFormatContext, "", nullptr, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to open AV format context: ", avErrorCodeToString(avError)));

    if ((avError = avformat_find_stream_info(_avFormatContext, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to find stream info: ", avErrorCodeToString(avError)));

    for (auto i = 0; i < _avFormatContext->nb_streams; i++) {
        if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            continue;

        _videoStreamIdx = i;
        break;
    }

    if (_videoStreamIdx == -1)
        throw std::runtime_error("Could not locate video stream.");

    for (auto i = 0; i < _avFormatContext->nb_streams; i++) {
        if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
            continue;

        _audioStreamIdx = i;
        break;
    }

    _microsPerFrame = static_cast<uint64_t>(1000000 / static_cast<float>(_avFormatContext->streams[_videoStreamIdx]->r_frame_rate.num) /
                                            static_cast<float>(_avFormatContext->streams[_videoStreamIdx]->r_frame_rate.den));

    const auto videoCodecPar = _avFormatContext->streams[_videoStreamIdx]->codecpar;
    const auto videoDecoder = avcodec_find_decoder(videoCodecPar->codec_id);

    if (videoDecoder == nullptr)
        throw std::runtime_error("Missing video codec.");

    _videoCodecContext = avcodec_alloc_context3(videoDecoder);
    if ((avError = avcodec_parameters_to_context(_videoCodecContext, videoCodecPar)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to apply parameters to video context: ", avErrorCodeToString(avError)));

    if ((avError = avcodec_open2(_videoCodecContext, videoDecoder, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to open video context: ", avErrorCodeToString(avError)));

    if (_audioStreamIdx >= 0) {
        const auto audioCodecPar = _avFormatContext->streams[_audioStreamIdx]->codecpar;
        const auto audioDecoder = avcodec_find_decoder(audioCodecPar->codec_id);

        if (audioDecoder == nullptr)
            throw std::runtime_error("Missing audio codec.");

        _audioCodecContext = avcodec_alloc_context3(audioDecoder);
        if ((avError = avcodec_parameters_to_context(_audioCodecContext, audioCodecPar)) < 0)
            throw std::runtime_error(absl::StrCat("Failed to apply parameters to audio context: ", avErrorCodeToString(avError)));

        if ((avError = avcodec_open2(_audioCodecContext, audioDecoder, nullptr)) < 0)
            throw std::runtime_error(absl::StrCat("Failed to open audio context: ", avErrorCodeToString(avError)));

        _resampleContext = swr_alloc();
        av_opt_set_int(_resampleContext, "in_channel_layout", static_cast<int64_t>(_audioCodecContext->channel_layout), 0);
        av_opt_set_int(_resampleContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(_resampleContext, "in_sample_rate", _audioCodecContext->sample_rate, 0);
        av_opt_set_int(_resampleContext, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(_resampleContext, "in_sample_fmt", _audioCodecContext->sample_fmt, 0);
        av_opt_set_sample_fmt(_resampleContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

        if ((avError = swr_init(_resampleContext)) < 0)
            throw std::runtime_error(absl::StrCat("Failed to initialize sound re-sampler: ", avErrorCodeToString(avError)));
    }

    const auto ratio = static_cast<float>(_videoCodecContext->height) / static_cast<float>(_videoCodecContext->width);

    _sourceRect = {.x = 0, .y = 0, .w = _videoCodecContext->width, .h = _videoCodecContext->height};
    _targetRect = {.x = 0, .y = (600 / 2) - static_cast<int>(800 * ratio / 2), .w = 800, .h = static_cast<int>(800 * ratio)};

    //) = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::YUV, _videoCodecContext->width, _videoCodecContext->height);
    _texture.reset(SDL_CreateTexture(Abyss::Singletons::getRendererProvider().getRenderer(), SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                     _videoCodecContext->width, _videoCodecContext->height));

    _swsContext = sws_getContext(_videoCodecContext->width, _videoCodecContext->height, _videoCodecContext->pix_fmt, _videoCodecContext->width,
                                 _videoCodecContext->height, AV_PIX_FMT_YUV420P, SWS_POINT, nullptr, nullptr, nullptr);

    const size_t yPlaneSize = _videoCodecContext->width * _videoCodecContext->height;
    const size_t uvPlaneSize = _videoCodecContext->width * _videoCodecContext->height / 4;
    _yPlane.resize(yPlaneSize);
    _uPlane.resize(uvPlaneSize);
    _vPlane.resize(uvPlaneSize);
    _uvPitch = _videoCodecContext->width / 2;

    _avFrame = av_frame_alloc();
    _videoTimestamp = av_gettime();

    // Engine::Get()->GetSystemIO().SetVideo(this);
    if (separateAudio) {
        _separateAudio = std::make_unique<Abyss::Streams::AudioStream>(*std::move(separateAudio));
        _separateAudio->play();
    }
}

VideoStream::~VideoStream() {
    // Engine::Get()->GetSystemIO().SetVideo(nullptr);

    av_free(_avioContext->buffer);
    avio_context_free(&_avioContext);
    avcodec_free_context(&_videoCodecContext);
    sws_freeContext(_swsContext);
    if (_audioStreamIdx >= 0) {
        avcodec_free_context(&_audioCodecContext);
        swr_free(&_resampleContext);
    }
    av_frame_free(&_avFrame);
    avformat_close_input(&_avFormatContext);
    avformat_free_context(_avFormatContext);
}

void VideoStream::update(const uint32_t ticks) {
    _totalTicks += ticks;
    while (_isPlaying) {
        if (const auto diff = av_gettime() - _videoTimestamp; diff < _microsPerFrame)
            break;

        _videoTimestamp += _microsPerFrame;
        while (!processFrame()) {
        }
    }
}

void VideoStream::render() const {
    if (!_framesReady)
        return;
    SDL_RenderCopy(Singletons::getRendererProvider().getRenderer(), _texture.get(), &_sourceRect, &_targetRect);
}

void VideoStream::stopVideo() { _isPlaying = false; }

short VideoStream::getAudioSample() {
    uint8_t data[2] = {};
    _ringBuffer.readData(std::span(data, 2));
    auto sample = static_cast<int16_t>(static_cast<uint16_t>(data[0] & 0xFF) | static_cast<uint16_t>(data[1]) << 8);
    if (_separateAudio) {
        sample += _separateAudio->getSample();
    }
    return sample;
}

bool VideoStream::getIsPlaying() const { return _isPlaying; }

} // namespace Abyss::Streams
