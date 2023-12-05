#include "DC6.h"

#include <Abyss/AbyssEngine.h>
#include <array>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>

namespace Abyss::DataTypes {

DC6Frame::DC6Frame(Streams::InputStream &stream)
    : _flipped(0), _width(0), _height(0), _xOffset(0), _yOffset(0), _unknown(0), _nextBlock(0), _length(0), _frameData(), _terminator() {
    stream.read(reinterpret_cast<char *>(&_flipped), sizeof(_flipped));
    stream.read(reinterpret_cast<char *>(&_width), sizeof(_width));
    stream.read(reinterpret_cast<char *>(&_height), sizeof(_height));
    stream.read(reinterpret_cast<char *>(&_xOffset), sizeof(_xOffset));
    stream.read(reinterpret_cast<char *>(&_yOffset), sizeof(_yOffset));
    stream.read(reinterpret_cast<char *>(&_unknown), sizeof(_unknown));
    stream.read(reinterpret_cast<char *>(&_nextBlock), sizeof(_nextBlock));
    stream.read(reinterpret_cast<char *>(&_length), sizeof(_length));

    _frameData.resize(_length);
    stream.read(std::bit_cast<char *>(_frameData.data()), _length);

    _terminator.resize(DC6TerminatorSize);
    stream.read(std::bit_cast<char *>(_terminator.data()), DC6TerminatorSize);
}

auto DC6Frame::getFlipped() const -> uint32_t { return _flipped; }

auto DC6Frame::getWidth() const -> uint32_t { return _width; }

auto DC6Frame::getHeight() const -> uint32_t { return _height; }

auto DC6Frame::getXOffset() const -> int32_t { return _xOffset; }

auto DC6Frame::getYOffset() const -> int32_t { return _yOffset; }

auto DC6Frame::getUnknown() const -> uint32_t { return _unknown; }

auto DC6Frame::getNextBlock() const -> uint32_t { return _nextBlock; }

auto DC6Frame::getLength() const -> uint32_t { return _length; }

auto DC6Frame::getFrameData() const -> std::vector<std::byte> { return _frameData; }

auto DC6Frame::getTerminator() const -> std::vector<std::byte> { return _terminator; }

DC6::DC6(const std::string_view path)
    : _version(0), _flags(0), _encoding(0), _termination(), _directions(0), _framesPerDirection(0), _texture(nullptr, &SDL_DestroyTexture),
      _blendMode(Enums::BlendMode::None) {
    auto stream = AbyssEngine::getInstance().loadStream(path);
    stream.read(reinterpret_cast<char *>(&_version), sizeof(_version));
    stream.read(reinterpret_cast<char *>(&_flags), sizeof(_flags));
    stream.read(reinterpret_cast<char *>(&_encoding), sizeof(_encoding));
    stream.read(reinterpret_cast<char *>(&_termination), sizeof(_termination));
    stream.read(reinterpret_cast<char *>(&_directions), sizeof(_directions));
    stream.read(reinterpret_cast<char *>(&_framesPerDirection), sizeof(_framesPerDirection));

    const auto frameCount = _directions * _framesPerDirection;

    _framePointers.resize(frameCount);

    for (auto &framePointer : _framePointers) {
        stream.read(reinterpret_cast<char *>(&framePointer), sizeof(framePointer));
    }

    for ([[maybe_unused]] auto &framePointer : _framePointers) {
        _frames.emplace_back(stream);
    }
}

auto DC6::getVersion() const -> uint32_t { return _version; }

auto DC6::getFlags() const -> uint32_t { return _flags; }

auto DC6::getEncoding() const -> uint32_t { return _encoding; }

auto DC6::getTermination() const -> std::array<uint8_t, DC6TerminationSize> { return _termination; }

auto DC6::getDirections() const -> uint32_t { return _directions; }

auto DC6::getFramesPerDirection() const -> uint32_t { return _framesPerDirection; }

auto DC6::getFramePointers() const -> std::vector<uint32_t> { return _framePointers; }

auto DC6::setPalette([[maybe_unused]] const Palette &palette) -> void {
    enum class eScanlineType { EndOfLine, RunOfTransparentPixels, RunOfOpaquePixels };
    auto scanlineType = [](const std::byte b) -> eScanlineType {
        if (b == DC6EndOfScanline)
            return eScanlineType::EndOfLine;

        if (static_cast<int>(b & DC6EndOfScanline) > 0)
            return eScanlineType::RunOfTransparentPixels;

        return eScanlineType::RunOfOpaquePixels;
    };

    uint32_t textureWidth = 1;
    uint32_t textureHeight = 1;

    for (const auto &frame : _frames) {
        textureWidth += frame.getWidth();
        textureHeight = std::max(textureHeight, frame.getHeight());
    }

    _texture.reset(SDL_CreateTexture(AbyssEngine::getInstance().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                     static_cast<int>(textureWidth), static_cast<int>(textureHeight)));

    if (!_texture)
        throw std::runtime_error(SDL_GetError());

    void *pixels;
    int pitch;
    if (SDL_LockTexture(_texture.get(), nullptr, &pixels, &pitch))
        throw std::runtime_error(SDL_GetError());

    std::memset(pixels, 0, textureHeight * pitch);

    auto originX = 0;
    for (const auto &frame : _frames) {
        auto process = true;
        auto x = originX;
        auto y = frame.getHeight() - 1;
        auto offset = 0;

        const auto &frameData = frame.getFrameData();
        while (process) {
            const auto b = frameData[offset];
            offset++;

            switch (scanlineType(b)) {
            case eScanlineType::EndOfLine: {
                if (y == 0) {
                    process = false;
                    break;
                }
                y--;
                x = originX;
                break;
            }
            case eScanlineType::RunOfTransparentPixels: {
                const auto &transparentPixels = b & DC6MaxRunLength;
                x += static_cast<int>(transparentPixels);
                break;
            }
            case eScanlineType::RunOfOpaquePixels: {
                for (auto i = 0; i < static_cast<int>(b); i++) {
                    const auto paletteIndex = static_cast<int>(frameData[offset]);
                    const auto &paletteColor = palette.getEntries()[paletteIndex];
                    const auto pixel = static_cast<uint32_t>(paletteColor.getBlue() << 24 | paletteColor.getGreen() << 16 | paletteColor.getRed() << 8 | 0xFF);
                    *(static_cast<uint32_t *>(pixels) + y * (pitch / 4) + x + i) = pixel;

                    offset++;
                }
                x += static_cast<int>(b);
                break;
            }
            }
        }

        _frameRects.emplace_back(SDL_Rect{originX, 0, static_cast<int>(frame.getWidth()), static_cast<int>(frame.getHeight())});
        originX += static_cast<int>(frame.getWidth());
    }

    SDL_UnlockTexture(_texture.get());
    setBlendMode(_blendMode);
}

auto DC6::draw(const uint32_t frameIdx, const int x, const int y) const -> void {
    if (frameIdx >= _frameRects.size())
        throw std::runtime_error("Invalid frame index");

    const auto &frameRect = _frameRects[frameIdx];
    const auto &frame = _frames[frameIdx];
    const SDL_Rect destRect{x + frame.getXOffset(), y + frame.getYOffset() - frameRect.h, frameRect.w, frameRect.h};
    SDL_RenderCopy(AbyssEngine::getInstance().getRenderer(), _texture.get(), &frameRect, &destRect);
}

auto DC6::draw(uint32_t frameIdx, const int x, int y, const int framesX, const int framesY) const -> void {
    for (auto fy = 0; fy < framesY; fy++) {
        auto orgX = x;
        const auto yAdjust = _frameRects[frameIdx].h;
        const auto xAdjust = _frameRects[frameIdx].w;
        for (auto fx = 0; fx < framesX; fx++) {
            draw(frameIdx, orgX, y + yAdjust);
            frameIdx++;
            orgX += xAdjust;
        }
        y += yAdjust;
    }
}

auto DC6::setBlendMode(const Enums::BlendMode blendMode) -> void {
    this->_blendMode = blendMode;

    switch (blendMode) {
    default:
    case Enums::BlendMode::None:
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_NONE);
        break;
    case Enums::BlendMode::Blend:
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_BLEND);
        break;
    case Enums::BlendMode::Add:
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_ADD);
        break;
    case Enums::BlendMode::Mod:
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_MOD);
        break;
    }
}

auto DC6::getFrameSize(const uint32_t frameIdx, int &frameWidth, int &frameHeight) const -> void {
    if (frameIdx >= _frameRects.size())
        throw std::runtime_error("Invalid frame index");

    const auto &[x, y, w, h] = _frameRects[frameIdx];

    frameWidth = w;
    frameHeight = h;
}

auto DC6::getFrameCount() const -> uint32_t { return _framesPerDirection; }

} // namespace Abyss::DataTypes
