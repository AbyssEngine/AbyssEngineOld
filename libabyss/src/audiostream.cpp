#include <absl/strings/str_cat.h>
#include <libavutil/opt.h>
#include "libabyss/audiostream.h"

namespace {
const int DecodeBufferSize = 1024;
} // namespace

LibAbyss::AudioStream::AudioStream(std::unique_ptr<InputStream> stream) : _stream(std::move(stream)) {
    _avFormatContext = avformat_alloc_context();

    _avBuffer = (unsigned char *)av_malloc(DecodeBufferSize); // AVIO is going to free this automagically... because why not?
    memset(_avBuffer, 0, DecodeBufferSize);

    _avioContext =
        avio_alloc_context(_avBuffer, DecodeBufferSize, 0, this, &AudioStream::StreamReadCallback, nullptr, &AudioStream::StreamSeekCallback);

    _avFormatContext = avformat_alloc_context();
    _avFormatContext->pb = _avioContext;
    _avFormatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

    int avError;

    if ((avError = avformat_open_input(&_avFormatContext, "", nullptr, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to open AV format context: ", AvErrorCodeToString(avError)));

    if ((avError = avformat_find_stream_info(_avFormatContext, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to find stream info: ", AvErrorCodeToString(avError)));

    for (auto i = 0; i < _avFormatContext->nb_streams; i++) {
        if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
            continue;

        _audioStreamIdx = i;
        break;
    }

    if (_audioStreamIdx == 0)
        throw std::runtime_error("No audio stream found");

    const auto audioCodecPar = _avFormatContext->streams[_audioStreamIdx]->codecpar;
    auto audioDecoder = avcodec_find_decoder(audioCodecPar->codec_id);

    if (audioDecoder == nullptr)
        throw std::runtime_error("Missing audio codec.");

    _audioCodecContext = avcodec_alloc_context3(audioDecoder);
    if ((avError = avcodec_parameters_to_context(_audioCodecContext, audioCodecPar)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to apply parameters to audio context: ", AvErrorCodeToString(avError)));

    if ((avError = avcodec_open2(_audioCodecContext, audioDecoder, nullptr)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to open audio context: ", AvErrorCodeToString(avError)));

    _resampleContext = swr_alloc();
    av_opt_set_channel_layout(_resampleContext, "in_channel_layout", (int64_t)_audioCodecContext->channel_layout, 0);
    av_opt_set_channel_layout(_resampleContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(_resampleContext, "in_sample_rate", _audioCodecContext->sample_rate, 0);
    av_opt_set_int(_resampleContext, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(_resampleContext, "in_sample_fmt", _audioCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(_resampleContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    if ((avError = swr_init(_resampleContext)) < 0)
        throw std::runtime_error(absl::StrCat("Failed to initialize sound re-sampler: ", AvErrorCodeToString(avError)));

    _avFrame = av_frame_alloc();
}

LibAbyss::AudioStream::~AudioStream() {
    av_free(_avioContext->buffer);
    avio_context_free(&_avioContext);
//    avcodec_free_context(&_videoCodecContext);
//    sws_freeContext(_swsContext);
    if (_audioStreamIdx >= 0) {
        avcodec_free_context(&_audioCodecContext);
        swr_free(&_resampleContext);
    }
    av_frame_free(&_avFrame);
    avformat_free_context(_avFormatContext);
}

int16_t LibAbyss::AudioStream::GetSample() {
    return 0;
}

int LibAbyss::AudioStream::StreamRead(uint8_t *buffer, int size) {
    if (!_isPlaying)
        return 0;

    _stream->read((char *)buffer, size);
    return (int)_stream->gcount();
}

int64_t LibAbyss::AudioStream::StreamSeek(int64_t offset, int whence) {
    if (!_isPlaying)
        return -1;

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
    case AVSEEK_SIZE: {
        const auto curPos = _stream->tellg();
        _stream->seekg(0, std::ios_base::end);
        const auto endPos = _stream->tellg();
        _stream->seekg(curPos, std::ios_base::beg);
        return endPos;
    }
    default:
        return -1;
    }

    _stream->seekg(offset, dir);

    return 0;
}

std::string LibAbyss::AudioStream::AvErrorCodeToString(int avError) {
    char str[2048] = {};
    av_make_error_string(str, 2048, avError);
    return {str};
}
