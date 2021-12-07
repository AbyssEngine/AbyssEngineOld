#ifndef ABYSS_RINGBUFFER_H
#define ABYSS_RINGBUFFER_H

#include <cinttypes>
#include <vector>
#include <span>
#include <mutex>

namespace LibAbyss {
class RingBuffer {
  public:
    explicit RingBuffer(uint32_t bufferSize);
    void PushData(std::span<const uint8_t> data);
    void ReadData(std::span<uint8_t> outBuffer);
    void Reset();
    uint32_t Available();

  private:
    const uint32_t _bufferSize;

    uint32_t _writePosition;
    uint32_t _readPosition;
    uint32_t _remainingToRead;

    std::vector<uint8_t> _buffer;
    std::mutex _mutex;
};
}

#endif // ABYSS_RINGBUFFER_H
