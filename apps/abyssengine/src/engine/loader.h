#ifndef ABYSS_LOADER_H
#define ABYSS_LOADER_H

#include "provider.h"
#include <vector>
#include "libabyss/formats/d2/mpq.h"
#include <filesystem>
#include <istream>
#include "libabyss/streams/inputstream.h"

namespace AbyssEngine {

    class Loader {
    public:
        Loader();
        void AddProvider(std::unique_ptr<AbyssEngine::Provider> provider);
        bool FileExists(const std::filesystem::path &path);
        LibAbyss::InputStream Load(const std::filesystem::path &path);
    private:
        std::vector<std::unique_ptr<AbyssEngine::Provider>> _providers;
    };

}

#endif //ABYSS_LOADER_H
