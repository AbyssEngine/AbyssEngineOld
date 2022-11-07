#include "ttfmanager.h"
#include "engine.h"
#include <cstdio>
#include <fstream>
#include <ft2build.h>
#include <stdexcept>
#include <string>
#include FT_FREETYPE_H
#include <absl/strings/str_format.h>
#include <fontconfig/fcfreetype.h>
#include <pango/pangocairo.h>
#include <pango/pangofc-fontmap.h>

namespace AbyssEngine {

TtfManager::TtfManager() {
    auto stream = Engine::Get()->GetLoader().Load("/abyss-embedded/fontconfig.xml");
    std::string xml = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());

    _fcConfig = FcConfigCreate();

    FcConfigParseAndLoadFromMemory(_fcConfig, (const uint8_t *)xml.c_str(), true);
    // Just initialize FcSetApplication because FcConfigSetFonts is not available from outside fontconfig
    FcConfigAppFontAddFile(_fcConfig, (const FcChar8 *)"");

    _pangoFontMap = pango_cairo_font_map_new_for_font_type(CAIRO_FONT_TYPE_FT);
    pango_fc_font_map_set_config(PANGO_FC_FONT_MAP(_pangoFontMap), _fcConfig);
}

TtfManager::~TtfManager() {
    g_object_unref(_pangoFontMap);

    FcFontSetDestroy(FcConfigGetFonts(_fcConfig, FcSetApplication));
    FcConfigDestroy(_fcConfig);
    FcFini();

    for (const auto &[_, f] : _knownFonts) {
        std::filesystem::remove(f.second);
    }
}

namespace {
struct FTLib {
    FTLib() { FT_Init_FreeType(&ft); }
    ~FTLib() { FT_Done_FreeType(ft); }
    FT_Library ft = 0;
};

struct FTFace {
    ~FTFace() {
        if (face) {
            FT_Done_Face(face);
        }
    }
    FT_Face face = 0;
};
} // namespace

std::string TtfManager::AddFont(const std::filesystem::path &path) {
    std::string p = path.string();
    auto cached = _knownFonts.find(p);
    if (cached != _knownFonts.end()) {
        return cached->second.first;
    }

    std::string name;

    FcFontSet *set = FcConfigGetFonts(_fcConfig, FcSetApplication);

    auto stream = Engine::Get()->GetLoader().Load(p);

    std::istreambuf_iterator<char> eos;
    std::string str(std::istreambuf_iterator<char>(stream), eos);

    // Pango needs the font file to be a real file. Write it to tmp.
    std::string tmp = absl::StrCat(std::tmpnam(nullptr), path.extension().string());
    {
        std::ofstream ostr(tmp, std::ios::binary);
        std::copy(str.begin(), str.end(), std::ostreambuf_iterator<char>(ostr));
    }

    FTLib lib;
    FT_Long num_faces = 0;
    {
        FTFace face;
        if (int err = FT_New_Memory_Face(lib.ft, (const FT_Byte *)str.data(), str.length(), -1, &face.face)) {
            throw std::runtime_error(absl::StrFormat("Can't find number of faces in font %s: %d", p, err));
        }
        num_faces = face.face->num_faces;
    }

    for (FT_Long i = 0; i < num_faces; ++i) {
        FTFace face;
        if (int err = FT_New_Memory_Face(lib.ft, (const FT_Byte *)str.data(), str.length(), i, &face.face)) {
            throw std::runtime_error(absl::StrFormat("Can't get face %d in font %s: %d", i, p, err));
        }
        FcPattern *pat = FcFreeTypeQueryFace(face.face, (const FcChar8 *)tmp.c_str(), i, nullptr);
        // FcPatternPrint(pat);
        FcChar8 *fcname = FcPatternFormat(pat, (const FcChar8 *)"%{family[0]}");
        name = (const char *)fcname;
        free(fcname);
        if (!FcFontSetAdd(set, pat)) {
            FcPatternDestroy(pat);
            throw std::runtime_error(absl::StrFormat("Can't add face %d of font %s", i, p));
        }
        SPDLOG_INFO("Loaded font {} (face {}) as '{}'", p, i, name);
    }
    pango_fc_font_map_config_changed(PANGO_FC_FONT_MAP(_pangoFontMap));

    _knownFonts[p] = std::make_pair(name, std::filesystem::path(tmp));
    return name;
}

} // namespace AbyssEngine
