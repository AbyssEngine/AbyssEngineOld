#ifndef LIBABYSS_DC6_H
#define LIBABYSS_DC6_H

#include "inputstream.h"
#include "streamreader.h"
#include <cstdint>
#include <vector>

namespace LibAbyss {

class DC6 {
  public:
    class Direction {
      public:
        class Frame {
          public:
            explicit Frame(StreamReader &sr);
            uint32_t Flipped = 0;
            uint32_t Width = 0;
            uint32_t Height = 0;
            int32_t OffsetX = 0;
            int32_t OffsetY = 0;
            uint32_t Unknown = 0;
            uint32_t NextBlock = 0;
            uint32_t Length = 0;
            std::vector<uint8_t> FrameData;
            uint8_t Terminator[3] = {};
            std::vector<uint8_t> IndexData;
          private:
            void Decode();
            static uint8_t GetScanlineType(uint8_t b);
        };

        Direction() : Frames() {}
        std::vector<Frame> Frames;
    };

    explicit DC6(InputStream &stream);
    int32_t Version;
    uint32_t Flags;
    uint32_t Encoding;
    uint8_t Termination[4];
    uint32_t NumberOfDirections;
    uint32_t FramesPerDirection;
    std::vector<Direction> Directions;
};
} // namespace LibAbyss

#endif // LIBABYSS_DC6_H
