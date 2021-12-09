#ifndef ABYSS_VIDEO_H
#define ABYSS_VIDEO_H

#include "../systemio/interface.h"
#include "libabyss/inputstream.h"
#include "node.h"

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

namespace AbyssEngine {

class Video : public Node {
  public:
    Video(std::string_view name, LibAbyss::InputStream stream);
    ~Video() override;

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void MouseEventCallback(const MouseEvent &event) final;
    bool GetIsPlaying() const;
    void StopVideo();

    [[nodiscard]] std::string_view NodeType() const final { return "Video Node"; };

  private:
    int VideoStreamRead(uint8_t *buffer, int size);
    static int VideoStreamReadCallback(void *opaque, uint8_t *buffer, int size) { return ((Video *)opaque)->VideoStreamRead(buffer, size); };
    int64_t VideoStreamSeek(int64_t offset, int whence);
    static int64_t VideoStreamSeekCallback(void *opaque, int64_t offset, int whence) { return ((Video *)opaque)->VideoStreamSeek(offset, whence); };
    bool ProcessFrame();
    static std::string AvErrorCodeToString(int avError);

    LibAbyss::InputStream _stream;

    std::unique_ptr<ITexture> _videoTexture;

    AVFormatContext *_avFormatContext;
    AVIOContext *_avioContext;
    SwrContext *_resampleContext;
    AVCodecContext *_videoCodecContext;
    AVCodecContext *_audioCodecContext;
    AVFrame *_avFrame;
    uint8_t **_destData;
    int _lineSize;
    mutable std::mutex _isPlayingMutex;

    unsigned char *_avBuffer;
    std::vector<uint8_t> _yPlane;
    std::vector<uint8_t> _uPlane;
    std::vector<uint8_t> _vPlane;
    SwsContext *_swsContext;

    int _videoStreamIdx;
    int _audioStreamIdx;
    size_t _yPlaneSize;
    size_t _uvPlaneSize;
    int _uvPitch;
    uint64_t _microsPerFrame;
    uint64_t _videoTimestamp;
    Rectangle _sourceRect;
    Rectangle _targetRect;
    bool _isPlaying = true;
    bool _framesReady = false;
    uint32_t _totalTicks = 0;
};
} // namespace AbyssEngine

#endif // ABYSS_VIDEO_H
