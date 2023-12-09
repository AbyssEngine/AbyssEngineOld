#pragma once

#include <StormLib.h>
#include <filesystem>
#include <ios>
#include <string>

#define STORMLIB_NO_AUTO_LINK 1
#include "Abyss/Common/Logging.h"
#include "InputStream.h"
#include "Provider.h"

namespace Abyss::FileSystem {

class MPQStream final : public SizeableStreambuf {
    HANDLE _mpqFile = nullptr;
    std::streamsize _startOfBlock = 0;
    char _buffer[2048] = {};

  public:
    MPQStream(HANDLE mpq, const std::string &fileName);
    ~MPQStream() override { SFileCloseFile(_mpqFile); }
    std::streamsize StartOfBlockForTesting() const;

  protected:
    int underflow() override;
    pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    [[nodiscard]] std::streamsize size() const override;
};

class MPQ final : public Provider {
    HANDLE _stormMpq;

  public:
    /// Proxy constructor that creates an MPQ based on the specified filename.
    /// \param mpqPath Path to the MPQ file to load.
    explicit MPQ(const std::filesystem::path &mpqPath);
    ~MPQ() override;
    bool has(std::string_view fileName) override;
    InputStream load(std::string_view fileName) override;
    std::vector<std::string> fileList();
};

} // namespace Abyss::FileSystem
