#include "libabyss/streamreader.h"

LibAbyss::StreamReader::StreamReader(LibAbyss::InputStream &inputStream) : _inputStream(inputStream) {}

uint8_t LibAbyss::StreamReader::ReadByte() { return _inputStream.get(); }

void LibAbyss::StreamReader::ReadBytes(uint8_t *data, unsigned int size) {
    for (auto i = 0; i < size; i++) {
        data[i] = ReadByte();
    }
}
