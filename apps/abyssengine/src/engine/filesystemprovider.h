#ifndef ABYSS_FILESYSTEMPROVIDER_H
#define ABYSS_FILESYSTEMPROVIDER_H

#include "provider.h"
#include "libabyss/inputstream.h"

namespace AbyssEngine {

    class FileSystemProvider : public Provider {
    public:
        explicit FileSystemProvider(std::filesystem::path path);
        ~FileSystemProvider() override = default;

    protected:
        bool Exists(const std::filesystem::path &path) override;

        LibAbyss::InputStream Load(const std::filesystem::path &path) override;
    private:
        std::filesystem::path _basePath;
    };

}

#endif //ABYSS_FILESYSTEMPROVIDER_H
