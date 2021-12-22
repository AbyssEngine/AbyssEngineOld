#ifndef ABYSS_PNGLOADER_H
#define ABYSS_PNGLOADER_H

#include "libabyss/streams/inputstream.h"
#include <vector>
#include <span>
#include <cinttypes>

namespace LibAbyss {

class PNGLoader {
  public:
    explicit PNGLoader(InputStream &stream);
    std::span<uint32_t> GetPixelData();
    void GetSize(uint32_t &width, uint32_t &height) const;

  private:
    std::vector<uint32_t> _pixelData;
    uint32_t _width;
    uint32_t _height;
};

} // namespace LibAbyss

#endif // ABYSS_PNGLOADER_H
