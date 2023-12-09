#pragma once

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <span>
#include <stdexcept>
#include <vector>

namespace Abyss::Common {

template <typename T> class RingBuffer {
    const uint32_t _bufferSize;
    uint32_t _writePosition;
    uint32_t _readPosition;
    uint32_t _remainingToRead;
    std::vector<T> _buffer;
    std::mutex _mutex;

  public:
    RingBuffer(uint32_t bufferSize) : _bufferSize(bufferSize), _writePosition(0), _readPosition(0), _remainingToRead(0), _buffer(bufferSize) {}

    uint32_t available() const { return _remainingToRead; }

    uint32_t capacity() const { return _bufferSize; }

    bool empty() const { return _remainingToRead == 0; }

    bool full() const { return _remainingToRead == _bufferSize; }

    void clear() {
        std::lock_guard lock(_mutex);
        _writePosition = 0;
        _readPosition = 0;
        _remainingToRead = 0;
    }

    void readData(std::span<T> outBuffer) {
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

    void pushData(std::span<T> data) {
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
