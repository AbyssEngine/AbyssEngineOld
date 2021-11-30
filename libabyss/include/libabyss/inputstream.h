#ifndef LIBABYSS_INPUTSTREAM_H
#define LIBABYSS_INPUTSTREAM_H

#include <istream>
#include <memory>
#include <streambuf>

namespace LibAbyss {
class InputStream : public std::istream {
  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()), _streamBuff(std::move(streamBuff)) {}

    InputStream(InputStream &&other) noexcept : std::istream(other._streamBuff.get()), _streamBuff(std::move(other._streamBuff)) {}

  private:
    std::unique_ptr<std::streambuf> _streamBuff;
};
} // namespace LibAbyss

#endif // LIBABYSS_INPUTSTREAM_H
