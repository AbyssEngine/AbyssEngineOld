module;

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <span>
#include <stdexcept>
#include <vector>

export module Abyss.Common.RingBuffer;

namespace Abyss::Common {

export template <typename T> class RingBuffer {
    const uint32_t _bufferSize;
    uint32_t _writePosition;
    uint32_t _readPosition;
    uint32_t _remainingToRead;
    std::vector<T> _buffer;
    std::mutex _mutex;

  public:
    explicit RingBuffer(uint32_t bufferSize) : _bufferSize(bufferSize), _writePosition(0), _readPosition(0), _remainingToRead(0), _buffer(bufferSize) {}

    [[nodiscard]] auto available() const -> uint32_t { return _remainingToRead; }

    [[nodiscard]] auto capacity() const -> uint32_t { return _bufferSize; }

    [[nodiscard]] auto empty() const -> bool { return _remainingToRead == 0; }

    [[nodiscard]] auto full() const -> bool { return _remainingToRead == _bufferSize; }

    auto clear() -> void {
        std::lock_guard lock(_mutex);
        _writePosition = 0;
        _readPosition = 0;
        _remainingToRead = 0;
    }

    auto readData(std::span<T> outBuffer) -> void {
        std::lock_guard lock(_mutex);

        const auto bla = outBuffer.size();
        const auto toRead = std::min((uint32_t)outBuffer.size(), _remainingToRead);

        if (outBuffer.empty() || toRead == 0)
            return;

        std::fill(outBuffer.begin(), outBuffer.end(), 0);
        auto readPos = _readPosition;

        while (readPos >= _bufferSize)
            readPos -= _bufferSize;

        for (auto i = 0; i < toRead; i++) {
            outBuffer[i] = _buffer[readPos++];

            while (readPos >= _bufferSize)
                readPos -= _bufferSize;
        }

        _readPosition = readPos;
        _remainingToRead -= toRead;
    }

    auto pushData(std::span<T> data) {
        std::lock_guard lock(_mutex);

        const auto remainingSize = _bufferSize - _remainingToRead;
        const auto toWrite = data.size();

        if (toWrite > remainingSize)
            throw std::runtime_error("RingBuffer overflow");

        auto writePos = _writePosition;

        while (writePos >= _bufferSize)
            writePos -= _bufferSize;

        for (auto i = 0; i < (int)toWrite; i++) {
            _buffer[writePos++] = data[i];

            while (writePos >= _bufferSize)
                writePos -= _bufferSize;
        }

        _remainingToRead += toWrite;
        _writePosition = writePos;

        while (_readPosition >= _bufferSize)
            _readPosition -= _bufferSize;
    }
};

} // namespace Abyss::Common
