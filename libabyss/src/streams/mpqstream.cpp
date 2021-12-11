#include "libabyss/streams/mpqstream.h"
#include <absl/strings/str_format.h>
#include <ios>

#define STORMLIB_NO_AUTO_LINK 1
#include <StormLib.h>
#include <spdlog/spdlog.h>

namespace LibAbyss {

MPQStream::MPQStream(HANDLE mpq, const std::string &fileName) {
    if (!SFileOpenFileEx(mpq, fileName.c_str(), SFILE_OPEN_FROM_MPQ, &_mpqFile)) {
        throw std::runtime_error(absl::StrFormat("Failed to open file '%s' from MPQ", fileName));
    }
}

MPQStream::~MPQStream() { SFileCloseFile(_mpqFile); }

int MPQStream::underflow() {
    if (gptr() == egptr()) {
        _startOfBlock += egptr() - eback();
        DWORD amountRead;
        if (!SFileReadFile(_mpqFile, _buffer, sizeof(_buffer), &amountRead, nullptr)) {
            if (GetLastError() != ERROR_HANDLE_EOF) {
                throw std::runtime_error("Error reading file from MPQ");
            }
        }
        setg(_buffer, _buffer, _buffer + amountRead);
    }

    return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
}

MPQStream::pos_type MPQStream::seekpos(pos_type pos, std::ios_base::openmode which) { return seekoff(pos, std::ios_base::beg, which); }

MPQStream::pos_type MPQStream::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) {
    std::streamsize newPos = 0;
    switch (dir) {
    case std::ios_base::beg:
        newPos = off;
        break;
    case std::ios_base::cur:
        newPos = _startOfBlock + (gptr() - eback()) + off;
        break;
    case std::ios_base::end:
        newPos = size() + off;
        break;
    default:
        break;
    }
    if (newPos >= _startOfBlock && newPos < _startOfBlock + (egptr() - eback())) {
        // The new position is already in the buffer, just repoint the pointer to it
        setg(eback(), eback() + newPos - _startOfBlock, egptr());
    } else {
        // Drop buffer, it will be read in underflow
        SFileSetFilePointer(_mpqFile, (int)newPos, nullptr, 0);
        setg(nullptr, nullptr, nullptr);
        _startOfBlock = newPos;
    }
    return _startOfBlock + (gptr() - eback());
}

std::streamsize MPQStream::size() const {
    return SFileGetFileSize(_mpqFile, nullptr);
}

} // namespace LibAbyss
