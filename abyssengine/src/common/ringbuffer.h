#ifndef ABYSS_RINGBUFFER_H
#define ABYSS_RINGBUFFER_H

#include <cinttypes>
#include <vector>
#include <span>
#include <mutex>

namespace AbyssEngine {
class RingBuffer {
  public:
    explicit RingBuffer(uint32_t bufferSize);
    void PushData(std::span<uint8_t> data);
    void ReadData(uint8_t *outBuffer, uint32_t size);
    void Reset();

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
