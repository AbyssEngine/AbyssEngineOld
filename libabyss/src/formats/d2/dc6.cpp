#include "libabyss/formats/d2/dc6.h"
#include "libabyss/streams/streamreader.h"
#include <stdexcept>

namespace LibAbyss {
namespace {
constexpr uint8_t EndOfScanline = 0x80;
constexpr uint8_t MaxRunLength = 0x7F;
enum ByteType {
    EndOfLine = 1,
    RunOfTransparentPixels,
    RunOfOpaquePixels,
};

ByteType GetScanlineType(uint8_t b) {
    if (b == EndOfScanline)
        return EndOfLine;

    if ((b & EndOfScanline) > 0)
        return RunOfTransparentPixels;

    return RunOfOpaquePixels;
}
} // namespace

DC6::DC6(InputStream &stream) : Termination() {
    StreamReader sr(stream);

    Version = sr.ReadInt32();
    Flags = sr.ReadUInt32();
    Encoding = sr.ReadUInt32();
    sr.ReadBytes(Termination);

    NumberOfDirections = sr.ReadUInt32();
    FramesPerDirection = sr.ReadUInt32();

    Directions.reserve(NumberOfDirections);
    auto totalFrames = NumberOfDirections * FramesPerDirection;

    std::vector<uint32_t> pointers;
    for (unsigned int i = 0; i < totalFrames; ++i) {
        pointers.push_back(sr.ReadUInt32());
    }

    uint32_t num = 0;
    for (auto directionIndex = 0; directionIndex < NumberOfDirections; directionIndex++) {
        Direction direction;
        direction.Frames.reserve(FramesPerDirection);

        for (auto frameIndex = 0; frameIndex < FramesPerDirection; frameIndex++) {
            stream.clear();
            stream.seekg(pointers[num++], std::ios_base::beg);
            direction.Frames.emplace_back(sr);
            if (stream.eof())
                throw std::runtime_error("EOF while decoding DC6.");
        }

        Directions.push_back(direction);
    }
}

DC6::Direction::Frame::Frame(StreamReader &sr) {
    Flipped = sr.ReadUInt32();
    Width = sr.ReadUInt32();
    Height = sr.ReadUInt32();
    OffsetX = sr.ReadInt32();
    OffsetY = sr.ReadInt32();
    Unknown = sr.ReadUInt32();
    NextBlock = sr.ReadUInt32();
    Length = sr.ReadUInt32();
    IndexData.resize(Width * Height);
    Decode(sr);
}
void DC6::Direction::Frame::Decode(StreamReader &sr) {
    uint32_t x = 0;
    uint32_t y = Height - 1;
    uint32_t endy = 0;
    if (Flipped)
        std::swap(y, endy);
    int dy = Flipped ? 1 : -1;
    uint32_t offset = 0;

    for (;;) {
        if (offset >= Length)
            throw std::runtime_error("Data overrun while decoding DC6 frame.");

        auto b = sr.ReadUInt8();
        offset++;

        switch (GetScanlineType(b)) {
        case EndOfLine:
            if (y == endy)
                goto done;

            y += dy;
            x = 0;
            continue;
        case RunOfTransparentPixels:
            x += (b & MaxRunLength);
            continue;
        case RunOfOpaquePixels:
            if (b == 0) {
                // Some D2R DC6 files have data starting with zeros but going over the Length into the padding
                offset--;
                continue;
            }
            for (int i = 0; i < b; i++) {
                if (offset >= Length)
                    throw std::runtime_error("Data overrun while decoding DC6 frame.");

                if ((x + (y * Width) + i) >= (Width * Height))
                    throw std::runtime_error("X/Y position out of bounds while decoding DC6 frame.");

                IndexData[x + (y * Width) + i] = sr.ReadUInt8();
                offset++;
            }
            x += b;
            continue;
        }
    }

done:
    if (offset != Length)
        throw std::runtime_error("Invalid DC6 frame length.");
    return;
}

} // namespace LibAbyss
