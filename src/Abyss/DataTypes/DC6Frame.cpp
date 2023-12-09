#include "DC6Frame.h"
#include <bit>


namespace Abyss::DataTypes {

DC6Frame::DC6Frame(FileSystem::InputStream &stream) {
    stream.read(reinterpret_cast<char *>(&_flipped), sizeof(_flipped));
    stream.read(reinterpret_cast<char *>(&_width), sizeof(_width));
    stream.read(reinterpret_cast<char *>(&_height), sizeof(_height));
    stream.read(reinterpret_cast<char *>(&_xOffset), sizeof(_xOffset));
    stream.read(reinterpret_cast<char *>(&_yOffset), sizeof(_yOffset));
    stream.read(reinterpret_cast<char *>(&_unknown), sizeof(_unknown));
    stream.read(reinterpret_cast<char *>(&_nextBlock), sizeof(_nextBlock));
    stream.read(reinterpret_cast<char *>(&_length), sizeof(_length));

    _frameData.resize(_length);
    stream.read(std::bit_cast<char *>(_frameData.data()), _length);

    _terminator.resize(DC6TerminatorSize);
    stream.read(std::bit_cast<char *>(_terminator.data()), DC6TerminatorSize);
}

uint32_t DC6Frame::getFlipped() const { return _flipped; }

uint32_t DC6Frame::getWidth() const { return _width; }

uint32_t DC6Frame::getHeight() const { return _height; }

int32_t DC6Frame::getXOffset() const { return _xOffset; }

int32_t DC6Frame::getYOffset() const { return _yOffset; }

uint32_t DC6Frame::getUnknown() const { return _unknown; }

uint32_t DC6Frame::getNextBlock() const { return _nextBlock; }

uint32_t DC6Frame::getLength() const { return _length; }

std::vector<std::byte> DC6Frame::getFrameData() const { return _frameData; }

std::vector<std::byte> DC6Frame::getTerminator() const { return _terminator; }

} // namespace Abyss::DataTypes
