#ifndef ABYSS_BYTEARRAYSTREAM_H
#define ABYSS_BYTEARRAYSTREAM_H

#include "libabyss/streams/inputstream.h"
#include <ios>
#include <streambuf>
#include <vector>

namespace LibAbyss {

class ByteArrayStream : public SizeableStreambuf {
  public:
    explicit ByteArrayStream(std::vector<uint8_t> data);
    ~ByteArrayStream() override;

    [[nodiscard]] std::streamsize size() const override;
    int underflow() override;
    pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;

  private:
    std::vector<uint8_t> _data;
    uint32_t _pos = 0;
};

} // namespace LibAbyss

#endif // ABYSS_BYTEARRAYSTREAM_H
