#ifndef ABYSS_PROVIDER_H
#define ABYSS_PROVIDER_H

#include <filesystem>
#include "libabyss/inputstream.h"

namespace AbyssEngine {

    class Provider {
    public:
        virtual ~Provider() = default;
        virtual bool Exists(const std::filesystem::path &path) = 0;
        virtual LibAbyss::InputStream Load(const std::filesystem::path &path) = 0;
    };

}

#endif //ABYSS_PROVIDER_H
