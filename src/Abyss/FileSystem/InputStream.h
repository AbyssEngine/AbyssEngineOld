#pragma once
#include <ios>
#include <istream>

namespace Abyss::FileSystem {

class SizeableStreambuf : public std::basic_streambuf<char> {
  public:
    [[nodiscard]] virtual auto size() const -> std::streamsize = 0;
};

class InputStream final : public std::istream {
    std::unique_ptr<std::streambuf> _streamBuff;

  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff);
    InputStream(InputStream &&other) noexcept;
    std::streamsize size();
    bool eof();
    template <typename ReadT> ReadT readValue() {
        ReadT value;
        read(reinterpret_cast<char *>(&value), sizeof(ReadT));
        return value;
    }
};

} // namespace Abyss::FileSystem
