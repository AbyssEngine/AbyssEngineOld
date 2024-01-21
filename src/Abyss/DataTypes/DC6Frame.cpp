#include "DC6Frame.h"
#include <bit>


namespace Abyss::DataTypes {

DC6Frame::DC6Frame(Streams::StreamReader &stream) {
  _flipped = stream.readUInt32();
  _width = stream.readUInt32();
  _height = stream.readUInt32();
  _xOffset = stream.readInt32();
  _yOffset = stream.readInt32();
  _unknown = stream.readUInt32();
  _nextBlock = stream.readUInt32();
  _length = stream.readUInt32();

  _frameData.resize(_length + DC6TerminatorSize);
  stream.readBytes(_frameData);
}

uint32_t DC6Frame::getFlipped() const { return _flipped; }

uint32_t DC6Frame::getWidth() const { return _width; }

uint32_t DC6Frame::getHeight() const { return _height; }

int32_t DC6Frame::getXOffset() const { return _xOffset; }

int32_t DC6Frame::getYOffset() const { return _yOffset; }

uint32_t DC6Frame::getUnknown() const { return _unknown; }

uint32_t DC6Frame::getNextBlock() const { return _nextBlock; }

uint32_t DC6Frame::getLength() const { return _length; }

const std::vector<std::byte>& DC6Frame::getFrameData() const { return _frameData; }

} // namespace Abyss::DataTypes
