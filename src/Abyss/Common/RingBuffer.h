#pragma once

#include <cstdint>
#include <mutex>
#include <span>
#include <vector>
#include <stdexcept>

namespace Abyss::Common {

template <typename T> class RingBuffer {
  private:
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
        std::lock_guard<std::mutex> lock(_mutex);
        _writePosition = 0;
        _readPosition = 0;
        _remainingToRead = 0;
    }

    auto readData(std::span<T> outBuffer) -> void {
        std::lock_guard<std::mutex> lock(_mutex);

        const auto toRead = std::min<T>(outBuffer.size(), _remainingToRead);
        std::fill(outBuffer.begin(), outBuffer.end(), 0);

        if (outBuffer.empty() || toRead == 0)
            return;

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
        std::lock_guard<std::mutex> lock(_mutex);

        const auto remainingSize = _bufferSize - _writePosition;
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