#include "spritefont.h"
#include "engine.h"
#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_split.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <unicode/chariter.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>

namespace AbyssEngine {
namespace {
const uint32_t MaxSpriteFontAtlasWidth = 1024;
}

SpriteFont::SpriteFont(std::string_view filePath, std::string_view paletteName, bool useGlyphHeight, eBlendMode
        blendMode)
    : _atlas(), _frameRects(), _palette(Engine::Get()->GetPalette(paletteName)), _useGlyphHeight(useGlyphHeight) {
    auto engine = Engine::Get();

    std::filesystem::path rootPath(filePath);
    auto dc6Path = std::filesystem::path(std::string(filePath) + ".dc6");

    if (!engine->GetLoader().FileExists(dc6Path))
        throw std::runtime_error("Could not locate font image path: " + dc6Path.string());

    {
        auto dataStream = engine->GetLoader().Load(dc6Path);
        _dc6 = std::make_unique<LibAbyss::DC6>(dataStream);
    }

    auto tblPath = std::filesystem::path(std::string(filePath) + ".tbl");
    if (!engine->GetLoader().FileExists(tblPath))
        throw std::runtime_error("Could not locate font table path: " + tblPath.string());

    auto dataStream = engine->GetLoader().Load(tblPath);
    LibAbyss::StreamReader sr(dataStream);
    uint8_t signature[5] = {};
    sr.ReadBytes(signature);
    if (std::string_view(reinterpret_cast<const char *>(signature), 5) != "Woo!\x01")
        throw std::runtime_error("Invalid signature on font table: " + tblPath.string());

    dataStream.ignore(7); // Skip unknown bytes

    while (!dataStream.eof()) {
        const auto code = sr.ReadUInt16();
        Glyph& glyph = _glyphs[code];

        dataStream.ignore(1); // Skip a byte for some reason

        glyph.Width = sr.ReadUInt8();
        glyph.Height = sr.ReadUInt8();

        dataStream.ignore(3); // Skip bytes for some reason

        glyph.FrameIndex = sr.ReadUInt16();

        if (glyph.FrameIndex != 0xFFFF && glyph.FrameIndex > _dc6->FramesPerDirection)
            throw std::runtime_error("Frame index out of range for sprite font.");

        dataStream.ignore(4); // Skip 4 bytes for some reason
    }
    _fallback = _glyphs.at('?');

    const auto addFallback = [&](uint16_t existing, uint16_t add) {
        auto it = _glyphs.find(existing);
        if (it == _glyphs.end()) return;
        Glyph g = it->second;
        _glyphs.try_emplace(add, g);
    };
    // Some kind of unicode dash
    addFallback('-', 8211);

    RegenerateAtlas();
    _atlas->SetBlendMode(blendMode);
}

void SpriteFont::RegenerateAtlas() {
    if (_dc6->NumberOfDirections != 1)
        throw std::runtime_error("Sprite had more than one direction, which is unexpected for fonts.");

    const auto &direction = _dc6->Directions[0];

    int atlasWidth = 0;
    int atlasHeight = 0;
    int curX = 0;
    unsigned int curHeight = 0;

    for (int frame_idx = 0; frame_idx < (int)_dc6->FramesPerDirection; frame_idx++) {
        const auto &frame = direction.Frames[frame_idx];

        if (curX + (int)frame.Width > MaxSpriteFontAtlasWidth) {
            atlasWidth = MaxSpriteFontAtlasWidth;

            atlasHeight += curHeight;
            curX = 0;
            curHeight = 0;
        }

        curX += (int)frame.Width;
        curHeight = std::max(curHeight, frame.Height);
    }

    atlasHeight += curHeight;

    _atlas = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::Static, atlasWidth, atlasHeight);
    _frameRects.clear();
    _frameRects.resize(_dc6->FramesPerDirection);
    std::vector<uint32_t> buffer;
    buffer.resize(atlasWidth * atlasHeight);

    int startX = 0;
    int startY = 0;
    curHeight = 0;

    for (auto frameIdx = 0; frameIdx < _dc6->FramesPerDirection; frameIdx++) {
        const auto &frame = direction.Frames[frameIdx];
        auto &frameRect = _frameRects[frameIdx];

        if (startX + (int)frame.Width > MaxSpriteFontAtlasWidth) {
            startX = 0;
            startY += curHeight;
            curHeight = 0;
        }

        frameRect.Rect.X = startX;
        frameRect.Rect.Y = startY;
        frameRect.Rect.Width = (int)frame.Width;
        frameRect.Rect.Height = (int)frame.Height;
        frameRect.OffsetX = frame.OffsetX;
        frameRect.OffsetY = frame.OffsetY;

        for (auto y = 0; y < frame.Height; y++) {
            for (auto x = 0; x < frame.Width; x++) {
                if (frame.IndexData[x + (y * frame.Width)] == 0)
                    continue;

                const auto &color = _palette.BasePalette[frame.IndexData[x + (y * frame.Width)]];
                const auto bufferIndex = startX + x + ((startY + y) * atlasWidth);

                if (bufferIndex >= buffer.size())
                    throw std::runtime_error("buffer index out of range");

                buffer[bufferIndex] = ((uint32_t)color.Red << 16) | ((uint32_t)color.Green << 8) | ((uint32_t)color.Blue) | ((uint32_t)0xFF << 24);
            }
        }

        startX += (int)frame.Width;
        curHeight = std::max(curHeight, frame.Height);

        _atlas->SetPixels(buffer);
    }
}
void SpriteFont::GetMetrics(std::string_view text, int &width, int &height) const {
    width = 0;
    height = 0;

    for (std::string_view line : absl::StrSplit(text, '\n')) {
        icu::UnicodeString us = icu::UnicodeString::fromUTF8(line);
        icu::UCharCharacterIterator it(us.getTerminatedBuffer(), us.length());
        bool first = _useGlyphHeight;
        int rowHeight = 0;
        int x = 0;
        it.setToStart();
        while (true) {
            UChar ch = it.nextPostInc();
            if (ch == icu::CharacterIterator::DONE) break;
            const auto it = _glyphs.find(ch);
            const auto& glyph = it == _glyphs.end() ? _fallback : it->second;
            const auto& frame = _frameRects[glyph.FrameIndex];
            if (first) {
                height += glyph.Height - frame.Rect.Height;
                first = false;
            }
            rowHeight = std::max(rowHeight, frame.Rect.Height);

            x += glyph.Width;
            width = std::max(x, width);
        }
        height += rowHeight;
    }
}

void SpriteFont::RenderText(int x, int y, std::string_view text, RGB colorMod, eAlignment
        horizontalAlignment) {
    _atlas->SetColorMod(colorMod.Red, colorMod.Green, colorMod.Blue);

    Rectangle targetRect{.X = x, .Y = y, .Width = 0, .Height = 0};
    int totalWidth = 0;
    switch (horizontalAlignment) {
    case eAlignment::Middle:
    case eAlignment::End: {
        int h;
        GetMetrics(text, totalWidth, h);
        break;
    }
    default:
        break;
    }

    for (std::string_view line : absl::StrSplit(text, '\n')) {
        bool first = _useGlyphHeight;
        int rowHeight = 0;
        int startX = x;
        switch (horizontalAlignment) {
        case eAlignment::Middle: {
            int w, h;
            GetMetrics(line, w, h);
            startX += (totalWidth - w) / 2;
            break;
        }
        case eAlignment::End: {
            int w, h;
            GetMetrics(line, w, h);
            startX += totalWidth - w;
            break;
        }
        default:
            break;
        }
        targetRect.X = startX;

        icu::UnicodeString us = icu::UnicodeString::fromUTF8(line);
        icu::UCharCharacterIterator it(us.getTerminatedBuffer(), us.length());
        it.setToStart();
        while (true) {
            UChar ch = it.nextPostInc();
            if (ch == icu::CharacterIterator::DONE) break;

            const auto it = _glyphs.find(ch);
            const auto& glyph = it == _glyphs.end() ? _fallback : it->second;
            const auto &frame = _frameRects[glyph.FrameIndex];
            if (first) {
                targetRect.Y += glyph.Height - frame.Rect.Height;
                first = false;
            }
            targetRect.Width = frame.Rect.Width;
            targetRect.Height = frame.Rect.Height;
            rowHeight = std::max(rowHeight, frame.Rect.Height);
            _atlas->Render(frame.Rect, targetRect);

            targetRect.X += glyph.Width;
        }

        targetRect.Y += rowHeight;
    }
}

} // namespace AbyssEngine
