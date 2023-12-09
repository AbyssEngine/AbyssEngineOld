#include "InputStream.h"

namespace Abyss::FileSystem {

InputStream::InputStream(std::unique_ptr<std::streambuf> streamBuff) : std::istream(streamBuff.get()), _streamBuff(std::move(streamBuff)) {}

InputStream::InputStream(InputStream &&other) noexcept : std::istream(other._streamBuff.get()), _streamBuff(std::move(other._streamBuff)) {}

std::streamsize InputStream::size() {
    if (const auto *sizeable = dynamic_cast<SizeableStreambuf *>(_streamBuff.get()))
        return sizeable->size();

    const auto curPos = tellg();
    seekg(0, end);
    const auto endPos = tellg();
    seekg(curPos, beg);
    return endPos;
}

bool InputStream::eof() { return tellg() >= size(); }

} // namespace Abyss::FileSystem
