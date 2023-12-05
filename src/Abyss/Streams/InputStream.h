#pragma once

#include <ios>
#include <istream>

namespace Abyss::Streams {

class SizeableStreambuf : public std::basic_streambuf<char> {
  public:
    [[nodiscard]] virtual auto size() const -> std::streamsize = 0;
};

class InputStream final : public std::istream {
    std::unique_ptr<std::streambuf> _streamBuff;

  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff);
    InputStream(InputStream &&other) noexcept;
    auto size() -> std::streamsize;
    auto eof() -> bool;
    template <typename ReadT> auto readValue() -> ReadT {
        ReadT value;
        read(reinterpret_cast<char *>(&value), sizeof(ReadT));
        return value;
    }
};

} // namespace Abyss::Streams
