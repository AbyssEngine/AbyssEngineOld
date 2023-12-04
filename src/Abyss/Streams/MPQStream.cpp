#include "MPQStream.h"
#include <Abyss/Common/Logging.h>
#include <Abyss/Extras/Blast/blast.h>
#include <Abyss/MPQ/Crypto.h>

namespace Abyss::Streams {

constexpr uint32_t BUFFER_SIZE = 2048;

MPQStream::MPQStream(const std::shared_ptr<MPQ::File> &mpqFile, std::string fileName)
    : mpqFile(mpqFile), fileName(std::move(fileName)), blockEntry(&mpqFile->getFileBlockData(this->fileName)), buffer(BUFFER_SIZE),
      vSize(0x200 << mpqFile->getHeader().blockSize) {
    uncompressedSize = blockEntry->getUncompressedSize();

    if (blockEntry->isCompressed() || blockEntry->isImploded() || !blockEntry->isSingleUnit()) {
        positions = mpqFile->getBlockOffsets(*blockEntry);
    }
}

auto MPQStream::loadBlock(const uint32_t blockIndex, const uint32_t expectedLength) -> void {
    uint32_t toRead;

    if (blockEntry->isCompressed() || blockEntry->isImploded()) {
        offset = positions.at(blockIndex);
        toRead = positions.at(blockIndex + 1) - offset;
    } else {
        offset = blockIndex * vSize;
        toRead = expectedLength;
    }

    offset += blockEntry->getFilePosition();
    buffer = mpqFile->readData(offset, toRead);

    if (blockEntry->isEncrypted() && blockEntry->getUncompressedSize() > 3) {
        if (blockEntry->getEncryptionSeed() == 0) {
            throw std::runtime_error("Unable to determine encryption key");
        }
        MPQ::Crypto::getInstance().decrypt(buffer, blockIndex + blockEntry->getEncryptionSeed());
    }

    if (blockEntry->isCompressed() && (toRead != expectedLength)) {
        if (!blockEntry->isSingleUnit()) {
            decompressMulti();
            return;
        }

        buffer.erase(buffer.begin());
        buffer = decompressPk(buffer);
        return;
    }

    if (blockEntry->isImploded() && (toRead != expectedLength)) {
        buffer.erase(buffer.begin());
        buffer = decompressPk(buffer);
        return;
    }
}

void MPQStream::decompressMulti() {
    switch (const auto compressionType = static_cast<uint8_t>(buffer.at(0))) {
    case 0x02: // ZLib/Deflate
        buffer = decompressZlib({buffer.data() + 1, buffer.size() - 1});
        break;
    case 0x08: // PKLib/Implode
        buffer = decompressPk({buffer.data() + 1, buffer.size() - 1});
        break;
    case 0x10: // BZip2
    case 0x80: // IMA ADPCM Stereo
    case 0x40: // IMA ADPCM Mono
    case 0x12: // LZMA
    case 0x22: // Sparse, then ZLib
    case 0x30: // Sparse, then BZip2
    case 0x41: // Huffman->WAV (1 channel)
    case 0x48: // PK + Wav (1 channel)
    case 0x81: // Huffman->WAV (2 channel)
    case 0x88: // PK + WAV (2 channel)
    default:
        Common::Log::error("Unsupported compression type: 0x{:02X}", compressionType);
        throw std::runtime_error("Unsupported compression type: " + std::to_string(compressionType));
    }
}

auto MPQStream::blastInFunction(void *how, unsigned char **buf) -> unsigned {
    auto *input = static_cast<std::vector<std::byte> *>(how);

    if (input->empty()) {
        *buf = nullptr;
        return 0;
    }

    *buf = reinterpret_cast<unsigned char *>(input->data());
    return static_cast<unsigned>(input->size());
}

auto MPQStream::blastOutFunction(void *how, unsigned char *buf, unsigned int len) -> int {
    auto *output = static_cast<std::vector<std::byte> *>(how);
    if (buf == nullptr || len <= 0) {
        return -1;
    }

    const size_t prevSize = output->size();
    const size_t newSize = prevSize + static_cast<size_t>(len);
    output->resize(newSize);
    std::memcpy(output->data() + prevSize, buf, len);

    return 0;
}

auto MPQStream::decompressZlib(const std::span<std::byte> &compressedData) -> std::vector<std::byte> {
    const size_t compressedSize = compressedData.size();
    std::vector<std::byte> decompressedBytes(4096);
    z_stream stream;
    stream.zalloc = nullptr;
    stream.zfree = nullptr;
    stream.opaque = nullptr;
    stream.avail_in = static_cast<uInt>(compressedSize);
    stream.next_in = const_cast<Bytef *>(reinterpret_cast<const Bytef *>(compressedData.data()));
    stream.avail_out = static_cast<uInt>(decompressedBytes.size());
    stream.next_out = reinterpret_cast<Bytef *>(decompressedBytes.data());

    if (inflateInit(&stream) != Z_OK) {
        throw std::runtime_error("Zlib initialization error");
    }

    int result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        throw std::runtime_error("Zlib decompression error:" + std::to_string(result));
    }

    decompressedBytes.resize(stream.total_out);
    inflateEnd(&stream);
    return decompressedBytes;
}

auto MPQStream::decompressPk(const std::span<std::byte> &compressedData) -> std::vector<std::byte> {
    std::vector<std::byte> decompressedData;
    unsigned inputBufferSize = 0;
    unsigned char *inputBuffer = nullptr;

    if (const BlastError result = blast(blastInFunction, (void *)&compressedData, blastOutFunction, &decompressedData, &inputBufferSize, &inputBuffer);
        result != BLAST_SUCCESS) {
        throw std::runtime_error("PKLib decompression error: " + std::to_string(result));
    }
    return decompressedData;
}

auto MPQStream::underflow() -> int {
    if (gptr() == egptr()) {
        startOfBlock += gptr() - eback();
        const auto blockIndex = startOfBlock / vSize;
        const auto expectedSize = std::min(vSize, static_cast<uint32_t>(blockEntry->getUncompressedSize() - (blockIndex * vSize)));
        loadBlock(static_cast<uint32_t>(blockIndex), expectedSize);
        setg(std::bit_cast<char *>(buffer.data()), std::bit_cast<char *>(buffer.data()), std::bit_cast<char *>(buffer.data()) + buffer.size());
    }

    return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
}

auto MPQStream::seekoff(const long long int off, const std::ios_base::seekdir dir, std::ios_base::openmode) -> pos_type {
    std::streamsize newPos = 0;
    switch (dir) {
    case std::ios_base::beg:
        newPos = off;
        break;
    case std::ios_base::cur:
        newPos = startOfBlock + (gptr() - eback()) + off;
        break;
    case std::ios_base::end:
        newPos = size() + off;
        break;
    default:
        break;
    }
    if (newPos >= startOfBlock && newPos < startOfBlock + (egptr() - eback())) {
        setg(eback(), eback() + newPos - startOfBlock, egptr());
    } else {
        setg(nullptr, nullptr, nullptr);
        startOfBlock = newPos;
    }
    return startOfBlock + (gptr() - eback());
}

auto MPQStream::seekpos(const std::fpos<mbstate_t> pos, const std::ios_base::openmode which) -> pos_type { return seekoff(pos, std::ios_base::beg, which); }

auto MPQStream::size() const -> std::streamsize { return static_cast<std::streamsize>(uncompressedSize); }

} // namespace Abyss::Streams
