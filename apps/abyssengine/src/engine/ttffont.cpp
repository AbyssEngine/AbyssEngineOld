#include "ttffont.h"
#include "engine.h"

namespace AbyssEngine {

TtfFont::TtfFont(std::string_view filePath, int size, ITtf::Hinting hinting) {
    auto *engine = AbyssEngine::Engine::Get();
    _ttf = engine->GetSystemIO().CreateTtf(engine->GetLoader().Load(filePath), size, hinting);
}

std::unique_ptr<ITexture> TtfFont::RenderText(std::string_view text, int &width, int &height) { return _ttf->RenderText(text, width, height); }

} // namespace AbyssEngine
