module;

#include <ios>
#include <istream>

export module Abyss.FileSystem.InputStream;

namespace Abyss::FileSystem {

export class SizeableStreambuf : public std::basic_streambuf<char> {
  public:
    [[nodiscard]] virtual auto size() const -> std::streamsize = 0;
};

export class InputStream final : public std::istream {
    std::unique_ptr<std::streambuf> _streamBuff;

  public:
    explicit InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()), _streamBuff(std::move(streamBuff)) {}

    InputStream(InputStream &&other) noexcept : std::istream(other._streamBuff.get()), _streamBuff(std::move(other._streamBuff)) {}

    auto size() -> std::streamsize {
        if (const auto *sizeable = dynamic_cast<SizeableStreambuf *>(_streamBuff.get()))
            return sizeable->size();

        const auto curPos = tellg();
        seekg(0, end);
        const auto endPos = tellg();
        seekg(curPos, beg);
        return endPos;
    }
    auto eof() -> bool { return tellg() >= size(); }

    template <typename ReadT> auto readValue() -> ReadT {
        ReadT value;
        read(reinterpret_cast<char *>(&value), sizeof(ReadT));
        return value;
    }
};

} // namespace Abyss::FileSystem
