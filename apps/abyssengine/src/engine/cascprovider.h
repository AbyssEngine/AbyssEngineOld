#ifndef ABYSS_CASCPROVIDER_H
#define ABYSS_CASCPROVIDER_H

#include "provider.h"
#include "libabyss/casc.h"

namespace AbyssEngine {

    class CASCProvider : public Provider {
    public:
        CASCProvider(const std::filesystem::path &path);

    protected:
        bool Exists(const std::filesystem::path &path) override;

        LibAbyss::InputStream Load(const std::filesystem::path &path) override;

    private:
        LibAbyss::CASC _casc;
    };

}


#endif //ABYSS_CASCPROVIDER_H
