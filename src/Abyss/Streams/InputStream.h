#pragma once

#include <ios>
#include <istream>
#include <memory>

namespace Abyss::Streams {

class SizeableStreambuf : public std::basic_streambuf<char> {
  public:
    [[nodiscard]] virtual auto size() const -> std::streamsize = 0;
};

class InputStream : public std::istream {
  private:
    std::unique_ptr<std::streambuf> _streamBuff;

  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff);

    InputStream(InputStream &&other) noexcept;

    InputStream(const InputStream &other) = delete;

    ~InputStream() override = default;

    auto operator=(const InputStream &other) -> InputStream & = delete;

    auto operator=(InputStream &&other) noexcept -> InputStream &;

    auto size() -> std::streamsize;
};

} // namespace Abyss::Streams
