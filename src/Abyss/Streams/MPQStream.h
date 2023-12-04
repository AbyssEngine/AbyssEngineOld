#pragma once

#include "InputStream.h"
#include <Abyss/MPQ/File.h>
#include <ios>
#include <span>
#include <string>
#include <vector>
#include <zlib.h>

namespace Abyss::Streams {

class MPQStream : public SizeableStreambuf {
    std::shared_ptr<MPQ::File> mpqFile;
    std::string fileName;
    size_t uncompressedSize;
    MPQ::MPQBlockEntry *blockEntry;
    std::vector<uint32_t> positions;
    std::streamsize startOfBlock{};
    std::vector<std::byte> buffer;
    uint32_t vSize;
    uint32_t offset{};

    auto loadBlock(uint32_t blockIndex, uint32_t expectedLength) -> void;
    void decompressMulti();
    static auto blastInFunction(void *how, unsigned char **buf) -> unsigned;
    static auto blastOutFunction(void *how, unsigned char *buf, unsigned len) -> int;
    static auto decompressZlib(const std::span<std::byte> &compressedData) -> std::vector<std::byte>;
    static auto decompressPk(const std::span<std::byte> &compressedData) -> std::vector<std::byte>;

  public:
    MPQStream(const std::shared_ptr<MPQ::File> &mpqFile, std::string fileName);
    auto underflow() -> int override;
    auto seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode /*which*/) -> MPQStream::pos_type override;
    auto seekpos(pos_type pos, std::ios_base::openmode which) -> MPQStream::pos_type override;
    [[nodiscard]] auto size() const -> std::streamsize override;
};

} // namespace Abyss::Streams
