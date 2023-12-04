#include "InputStream.h"

namespace Abyss::Streams {

InputStream::InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()), _streamBuff(std::move(streamBuff)) {}

InputStream::InputStream(InputStream &&other) noexcept : std::istream(other._streamBuff.get()), _streamBuff(std::move(other._streamBuff)) {}

auto InputStream::operator=(InputStream &&other) noexcept -> InputStream & {
    _streamBuff = std::move(other._streamBuff);
    rdbuf(_streamBuff.get());
    return *this;
}

auto InputStream::size() -> std::streamsize {
    if (auto *sizeable = dynamic_cast<SizeableStreambuf *>(_streamBuff.get())) {
        return sizeable->size();
    }
    const auto curPos = tellg();
    seekg(0, std::ios_base::end);
    const auto endPos = tellg();
    seekg(curPos, std::ios_base::beg);
    return endPos;
}

} // namespace Abyss::Streams
