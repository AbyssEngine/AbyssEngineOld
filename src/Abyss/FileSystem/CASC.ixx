module;

#include <filesystem>
#include <ios>
#include <string>

#define CASCLIB_NO_AUTO_LINK_LIBRARY 1
#include <CascLib.h>

export module Abyss.FileSystem.CASC;

import Abyss.FileSystem.InputStream;
import Abyss.FileSystem.Provider;
import Abyss.Common.Logging;

namespace Abyss::FileSystem {

export class CASC : public Provider {
  public:
    /// Proxy constructor that creates an CASC based on the specified filename.
    /// \param cascPath Path to the CASC dir to load.
    explicit CASC(const std::filesystem::path &cascPath);

    ~CASC();

    bool has(std::string_view fileName) override;
    InputStream load(std::string_view fileName) override;
    std::vector<std::string> fileList();

  private:
    void *_storage;
};

class CASCStream : public SizeableStreambuf {
  public:
    CASCStream(void *casc, std::string fileName);

    ~CASCStream() override;

  protected:
    int underflow() override;
    pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    std::streamsize size() const override;

  private:
    void *_file = 0;
    std::streamsize _startOfBlock = 0;
    char _buffer[2048] = {};
};

CASCStream::CASCStream(HANDLE storage, std::string fileName) {
    if (!CascOpenFile(storage, fileName.c_str(), 0, CASC_OPEN_BY_NAME, &_file)) {
        throw std::runtime_error("Failed to open file '" + fileName + "' from CASC");
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

} // namespace Abyss::FileSystem
