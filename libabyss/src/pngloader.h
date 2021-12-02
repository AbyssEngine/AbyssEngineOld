#ifndef ABYSS_PNGLOADER_H
#define ABYSS_PNGLOADER_H

#include "libabyss/inputstream.h"
#include <vector>
#include <span>
#include <cinttypes>

namespace LibAbyss {

class PNGLoader {
  public:
    explicit PNGLoader(InputStream &stream);
    std::span<uint32_t> GetPixelData();

  private:
    std::vector<uint32_t> _pixelData;
};

} // namespace LibAbyss

#endif // ABYSS_PNGLOADER_H
