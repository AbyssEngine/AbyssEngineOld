#include "libabyss/streams/cascstream.h"
#include <absl/strings/str_format.h>
#include <ios>

#define CASCLIB_NO_AUTO_LINK_LIBRARY 1
#include <CascLib.h>

namespace LibAbyss {

CASCStream::CASCStream(HANDLE storage, std::string fileName) {
    if (!CascOpenFile(storage, fileName.c_str(), 0, CASC_OPEN_BY_NAME, &_file)) {
        throw std::runtime_error(absl::StrFormat("Failed to open file '%s' from CASC", fileName));
    }
}

CASCStream::~CASCStream() { CascCloseFile(_file); }

int CASCStream::underflow() {
    if (gptr() == egptr()) {
        _startOfBlock += egptr() - eback();
        DWORD amountRead;
        if (!CascReadFile(_file, _buffer, sizeof(_buffer), &amountRead)) {
            if (GetCascError() != ERROR_HANDLE_EOF) {
                throw std::runtime_error("Error reading file from CASC");
            }
        }
        setg(_buffer, _buffer, _buffer + amountRead);
    }

    return gptr() == egptr() ? traits_type::eof() : traits_type::to_int_type(*gptr());
}

CASCStream::pos_type CASCStream::seekpos(pos_type pos, std::ios_base::openmode which) { return seekoff(pos, std::ios_base::beg, which); }

CASCStream::pos_type CASCStream::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) {
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
        CascSetFilePointer64(_file, newPos, nullptr, 0);
        setg(nullptr, nullptr, nullptr);
        _startOfBlock = newPos;
    }
    return _startOfBlock + (gptr() - eback());
}

std::streamsize CASCStream::size() const {
    ULONGLONG ulongsize;
    CascGetFileSize64(_file, &ulongsize);
    return ulongsize;
}

} // namespace LibAbyss
