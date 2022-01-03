#include "ttfmanager.h"
#include "engine.h"
#include <cstdio>
#include <fstream>
#include <string>

namespace AbyssEngine {

TtfManager::TtfManager() {
    auto stream = Engine::Get()->GetLoader().Load("/abyss-embedded/fontconfig.xml");
    std::string xml = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());

    FcInit();
    _fcConfig = FcConfigCreate();

    FcConfigParseAndLoadFromMemory(_fcConfig, (const uint8_t *)xml.c_str(), true);
    FcConfigSetCurrent(_fcConfig);
}

TtfManager::~TtfManager() {
    FcConfigDestroy(_fcConfig);
    FcFini();

    for (const auto &[_, f] : _knownFonts) {
        std::filesystem::remove(f);
    }
}

void TtfManager::AddFont(const std::filesystem::path &path) {
    std::string p = path.string();
    if (_knownFonts.contains(p))
        return;

    auto stream = Engine::Get()->GetLoader().Load(p);

    std::string tmp = absl::StrCat(std::tmpnam(nullptr), path.extension().string());

    std::ofstream ostr(tmp);
    std::copy(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(ostr));

    FcConfigAppFontAddFile(_fcConfig, (const FcChar8 *)tmp.c_str());

    _knownFonts[p] = std::filesystem::path(tmp);
}

} // namespace AbyssEngine
