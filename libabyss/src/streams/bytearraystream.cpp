#include "libabyss/streams/bytearraystream.h"
LibAbyss::ByteArrayStream::ByteArrayStream(std::vector<uint8_t> data) : _data(data) {}

LibAbyss::ByteArrayStream::~ByteArrayStream() = default;

std::streamsize LibAbyss::ByteArrayStream::size() const { return (std::streamsize)_data.size(); }

int LibAbyss::ByteArrayStream::underflow() {
    if (gptr() == egptr()) {
        _pos += egptr() - eback();
        setg((char *)_data.data(), (char *)_data.data(), (char *)_data.data() + _data.size());
    }

    return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
}

std::fpos<mbstate_t> LibAbyss::ByteArrayStream::seekpos(std::fpos<mbstate_t> pos, std::ios_base::openmode which) {
    return seekoff(pos, std::ios_base::beg, which);
}
std::fpos<mbstate_t> LibAbyss::ByteArrayStream::seekoff(long long int off, std::ios_base::seekdir dir, std::ios_base::openmode which) {
    std::streamsize newPos = 0;
    switch (dir) {
    case std::ios_base::beg:
        newPos = off;
        break;
    case std::ios_base::cur:
        newPos = _pos + (gptr() - eback()) + off;
        break;
    case std::ios_base::end:
        newPos = size() + off;
        break;
    default:
        break;
    }

    if (newPos >= _pos && newPos < _pos + (egptr() - eback())) {
        setg(eback(), eback() + newPos - _pos, egptr());
    } else {
        // Drop buffer, it will be read in underflow
        setg(nullptr, nullptr, nullptr);
        _pos = newPos;
    }
    return _pos + (gptr() - eback());
}
