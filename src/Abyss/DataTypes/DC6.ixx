module;

#include <SDL2/SDL.h>
#include <array>
#include <vector>

export module Abyss.DataTypes.DC6;

import Abyss.DataTypes.DC6Frame;
import Abyss.Singletons;
import Abyss.Enums.BlendMode;
import Abyss.DataTypes.Palette;
import Abyss.Common.Animation;

namespace Abyss::DataTypes {

export class DC6 {
    uint32_t _version;
    uint32_t _flags;
    uint32_t _encoding;
    std::array<uint8_t, DC6TerminationSize> _termination;
    uint32_t _directions;
    uint32_t _framesPerDirection;
    std::vector<uint32_t> _framePointers;
    std::vector<DC6Frame> _frames;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _texture;
    std::vector<SDL_Rect> _frameRects;
    Enums::BlendMode _blendMode;

  public:
    explicit DC6(const std::string_view path)
        : _version(0), _flags(0), _encoding(0), _termination(), _directions(0), _framesPerDirection(0), _texture(nullptr, &SDL_DestroyTexture),
          _blendMode(Enums::BlendMode::None) {
        auto stream = Singletons::getFileProvider().loadFile(path);
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

    DC6(const std::string_view path, const Palette &palette) : DC6(path) { setPalette(palette); }

    [[nodiscard]] auto getVersion() const -> uint32_t { return _version; }

    [[nodiscard]] auto getFlags() const -> uint32_t { return _flags; }

    [[nodiscard]] auto getEncoding() const -> uint32_t { return _encoding; }

    [[nodiscard]] auto getTermination() const -> std::array<uint8_t, DC6TerminationSize> { return _termination; }

    [[nodiscard]] auto getDirections() const -> uint32_t { return _directions; }

    [[nodiscard]] auto getFramesPerDirection() const -> uint32_t { return _framesPerDirection; }

    [[nodiscard]] auto getFramePointers() const -> std::vector<uint32_t> { return _framePointers; }

    [[nodiscard]] auto getFrameCount() const -> uint32_t { return _framesPerDirection; }

    auto setPalette([[maybe_unused]] const Palette &palette) -> void {
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

        _texture.reset(SDL_CreateTexture(Singletons::getRendererProvider().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
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
                        const auto pixel =
                            static_cast<uint32_t>(paletteColor.getBlue() << 24 | paletteColor.getGreen() << 16 | paletteColor.getRed() << 8 | 0xFF);
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

    auto draw(const uint32_t frameIdx, const int x, const int y) const -> void {
        if (frameIdx >= _frameRects.size())
            throw std::runtime_error("Invalid frame index");

        const auto &frameRect = _frameRects[frameIdx];
        const auto &frame = _frames[frameIdx];
        const SDL_Rect destRect{x + frame.getXOffset(), y + frame.getYOffset() - frameRect.h, frameRect.w, frameRect.h};
        SDL_RenderCopy(Singletons::getRendererProvider().getRenderer(), _texture.get(), &frameRect, &destRect);
    }

    auto draw(uint32_t frameIdx, const int x, int y, const int framesX, const int framesY) const -> void {
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

    auto setBlendMode(const Enums::BlendMode blendMode) -> void {
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

    auto getFrameSize(const uint32_t frameIdx, int &frameWidth, int &frameHeight) const -> void {
        if (frameIdx >= _frameRects.size())
            throw std::runtime_error("Invalid frame index");

        const auto &[x, y, w, h] = _frameRects[frameIdx];

        frameWidth = w;
        frameHeight = h;
    }

    auto getFrameOffset(const uint32_t frameIdx, int &offsetX, int &offsetY) const -> void {
        if (frameIdx >= _frames.size())
            throw std::runtime_error("Invalid frame index");

        const auto &frame = _frames[frameIdx];

        offsetX = frame.getXOffset();
        offsetY = frame.getYOffset();
    }
};

export typedef Common::Animation<DC6> DC6Animation;

} // namespace Abyss::DataTypes
