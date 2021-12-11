#include "libabyss/streams/streamreader.h"

LibAbyss::StreamReader::StreamReader(LibAbyss::InputStream &inputStream) : _inputStream(inputStream) {}

uint8_t LibAbyss::StreamReader::ReadByte() { return _inputStream.get(); }

void LibAbyss::StreamReader::ReadBytes(std::span<uint8_t> data) {
    if (data.empty()) {
        return;
    }
    _inputStream.read(reinterpret_cast<char *>(&data[0]), (long)data.size());
}
std::string LibAbyss::StreamReader::ReadString() {
    std::string result;
    char c;

    while ((c = (char)_inputStream.get()) != '\0')
        result.push_back(c);

    return result;
}
