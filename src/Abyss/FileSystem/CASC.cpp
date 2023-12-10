#include "CASC.h"
#include "Abyss/Common/Logging.h"
#include <algorithm>
#include <ios>
#include <format>
#include <ranges>

#define CASCLIB_NO_AUTO_LINK_LIBRARY 1
#include <CascLib.h>

namespace Abyss::FileSystem {

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

static bool casc_progress_callback(void *PtrUserParam, LPCSTR szWork, LPCSTR szObject, DWORD CurrentValue, DWORD TotalValue) {
    if (szObject) {
        if (TotalValue) {
            Common::Log::trace("CASC progress: {0} ({1}), {2}/{3}", szWork, szObject, CurrentValue, TotalValue);
        } else {
            Common::Log::trace("CASC progress: {0} ({1})", szWork, szObject);
        }
    } else {
        if (TotalValue) {
            Common::Log::trace("CASC progress: {0}, {1}/{2}", szWork, CurrentValue, TotalValue);
        } else {
            Common::Log::trace("CASC progress: {0}", szWork);
        }
    }
    return false;
}

CASC::CASC(const std::filesystem::path &cascPath) {
    std::string path = std::filesystem::absolute(cascPath).string();
    CASC_OPEN_STORAGE_ARGS args = {};
    args.Size = sizeof(CASC_OPEN_STORAGE_ARGS);
    args.PfnProgressCallback = casc_progress_callback;
    if (!CascOpenStorageEx(path.c_str(), &args, 0, &_storage)) {
        throw std::runtime_error(std::format("Error occurred while opening CASC {}: {}", cascPath.string(), GetCascError()));
    }
}

CASC::~CASC() { CascCloseStorage(_storage); }

static std::string FixPath(std::string_view str) {
    while (str.starts_with("/")) str = str.substr(1);
    while (str.starts_with("\\")) str = str.substr(1);
    return "data:" + std::string(str);
}

InputStream CASC::load(std::string_view fileName) { return InputStream(std::make_unique<CASCStream>(_storage, FixPath(fileName))); }

bool CASC::has(std::string_view fileName) {
    HANDLE file;
    if (CascOpenFile(_storage, FixPath(fileName).c_str(), 0, CASC_OPEN_BY_NAME, &file)) {
        CascCloseFile(file);
        return true;
    }
    return false;
}

} // namespace Abyss::FileSystem
