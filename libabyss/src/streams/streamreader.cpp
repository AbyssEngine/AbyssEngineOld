#include "libabyss/streams/streamreader.h"

namespace LibAbyss {

StreamReader::StreamReader(InputStream &inputStream) : _inputStream(inputStream) {}

uint8_t StreamReader::ReadByte() { return _inputStream.get(); }

void StreamReader::ReadBytes(std::span<uint8_t> data) {
    if (data.empty()) {
        return;
    }
    _inputStream.read(reinterpret_cast<char *>(&data[0]), (long)data.size());
}
std::string StreamReader::ReadString() {
    std::string result;
    char c;

    while ((c = (char)_inputStream.get()) != '\0')
        result.push_back(c);

    return result;
}

} // namespace LibAbyss
