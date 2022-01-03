#include "ttffont.h"
#include "engine.h"
#include <absl/strings/str_cat.h>
#include <cairomm/fontoptions.h>
#include <cairomm/surface.h>
#include <pango/pangocairo.h>
#include <pangomm.h>
#include <pangomm/init.h>

namespace AbyssEngine {

TtfFont::TtfFont(const std::filesystem::path &path, std::string_view name, int size, Cairo::HintStyle hint)
    : _name(absl::StrCat(name, " ", size)), _hint(hint) {
    auto *engine = AbyssEngine::Engine::Get();
    engine->GetTtfManager().AddFont(path);
}

std::unique_ptr<ITexture> TtfFont::RenderText(const std::string &text, int &width, int &height, eAlignment horizontalAlignment, int maxWidth) {
    Pango::init();

    Glib::RefPtr<Pango::FontMap> fontmap = Glib::wrap(pango_cairo_font_map_new());
    Glib::RefPtr<Pango::Context> pctx = fontmap->create_context();

    Cairo::FontOptions opts;
    opts.set_hint_style(_hint);
    opts.set_antialias(Cairo::ANTIALIAS_SUBPIXEL);
    pctx->set_cairo_font_options(opts);

    Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(pctx);
    layout->set_font_description(Pango::FontDescription(_name));

    Pango::Alignment align = Pango::ALIGN_LEFT;
    switch (horizontalAlignment) {
    case eAlignment::Middle:
        align = Pango::ALIGN_CENTER;
        break;
    case eAlignment::End:
        align = Pango::ALIGN_RIGHT;
        break;
    default:
        break;
    }
    layout->set_alignment(align);
    layout->set_width(maxWidth * PANGO_SCALE);

    layout->set_markup(text);
    Pango::Rectangle r = layout->get_pixel_logical_extents();
    width = r.get_width();
    height = r.get_height();

    Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
    cr->move_to(-r.get_x(), -r.get_y());

    layout->update_from_cairo_context(cr);
    layout->show_in_cairo_context(cr);

    // TODO use SDL_CreateRGBSurfaceWithFormatFrom
    auto texture = AbyssEngine::Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::BGRA, width, height);
    texture->SetPixels(std::span(surface->get_data(), surface->get_height() * surface->get_stride()), surface->get_stride());
    texture->SetBlendMode(eBlendMode::Blend);
    return texture;
}

} // namespace AbyssEngine
