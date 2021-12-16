#include "d2rsprite.h"
#include "../engine/engine.h"
#include <absl/strings/str_cat.h>
#include "libabyss/streams/streamreader.h"

namespace AbyssEngine {

D2RSprite::D2RSprite(LibAbyss::InputStream &stream) {
    LibAbyss::StreamReader sr(stream);
    stream.ignore(4);
    int version = sr.ReadUInt16();
    if (version != 31) {
        throw std::runtime_error(absl::StrCat("Unknown D2R sprite version ", version));
    }
    stream.ignore(2);
    _width_total = sr.ReadUInt32();
    _height = sr.ReadUInt32();
    stream.ignore(4);
    _frames = sr.ReadUInt32();
    stream.ignore(16);
    _data.resize(_width_total * _height * 4);
    sr.ReadBytes(_data);

    RegenerateAtlas();
}

void D2RSprite::GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) {}
void D2RSprite::GetFrameSize(uint32_t startFrameIdx, int cellSizeX, uint32_t &width, uint32_t &height) {
    width = _width_total / _frames;
    height = _height;
}
uint32_t D2RSprite::GetNumberOfAnimations() { return 1; }
uint32_t D2RSprite::GetFramesPerAnimation() { return _frames; }

void D2RSprite::RegenerateAtlas() {
    _atlas = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::Static, _width_total, _height);
    _atlas->SetPixels(_data);

    _framePositions.clear();
    int frame_width = _width_total / _frames;
    for (int i = 0; i < _frames; ++i) {
        _framePositions.push_back(FramePosition {
            .Rect = {
                .X = frame_width * i,
                .Y = 0,
                .Width = frame_width,
                .Height = (int)_height,
            },
            .OffsetX = 0,
            .OffsetY = 0,
        });
    }
}

}  // namespace AbyssEngine
