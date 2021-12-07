#ifndef ABYSS_LOADER_H
#define ABYSS_LOADER_H

#include "provider.h"
#include <vector>
#include "libabyss/mpq.h"
#include <mutex>
#include <filesystem>
#include <istream>
#include "libabyss/inputstream.h"

namespace AbyssEngine {

    class Loader {
    public:
        Loader();
        void AddProvider(std::unique_ptr<AbyssEngine::Provider> provider);
        bool FileExists(const std::filesystem::path &path);
        LibAbyss::InputStream Load(const std::filesystem::path &path);
    private:
        std::mutex _mutex;
        std::vector<std::unique_ptr<AbyssEngine::Provider>> _providers;
    };

}

#endif //ABYSS_LOADER_H
