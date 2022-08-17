#include "ttffont.h"
#include "engine.h"
#include <absl/strings/str_cat.h>
//#include <cairomm/fontoptions.h>
//#include <cairomm/surface.h>
#include <pango/pangocairo.h>
//#include <pangomm.h>
//#include <pangomm/init.h>
#include <absl/cleanup/cleanup.h>

namespace AbyssEngine {

TtfFont::TtfFont(const std::filesystem::path &path, int size, /*Cairo::HintStyle*/
        cairo_hint_style_t hint, cairo_antialias_t antialias)
    : _hint(hint), _antialias(antialias) {
    auto *engine = AbyssEngine::Engine::Get();
    _name = absl::StrCat(engine->GetTtfManager().AddFont(path), " ", size);
}

std::unique_ptr<ITexture> TtfFont::RenderText(const std::string &text, int &width, int &height, eAlignment horizontalAlignment, int maxWidth) {
    auto* font_map = AbyssEngine::Engine::Get()->GetTtfManager().FontMap();
    auto* pctx = pango_cairo_font_map_create_context(PANGO_CAIRO_FONT_MAP(font_map));
    absl::Cleanup clean_ctx([&] { g_object_unref(pctx); });

    auto* opts = cairo_font_options_create();
    absl::Cleanup clean_opts([&]{ cairo_font_options_destroy(opts); });
    cairo_font_options_set_hint_style(opts, _hint);
    cairo_font_options_set_antialias(opts, _antialias);
    pango_cairo_context_set_font_options(pctx, opts);

    auto* layout = pango_layout_new(pctx);
    absl::Cleanup clean_layout([&]{ g_object_unref(layout); });
    auto* desc = pango_font_description_from_string(_name.c_str());
    pango_font_description_set_weight(desc, PANGO_WEIGHT_BOLD);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    PangoAlignment align = PANGO_ALIGN_LEFT;
    switch (horizontalAlignment) {
    case eAlignment::Middle:
        align = PANGO_ALIGN_CENTER;
        break;
    case eAlignment::End:
        align = PANGO_ALIGN_RIGHT;
        break;
    default:
        break;
    }
    pango_layout_set_alignment(layout, align);
    pango_layout_set_width(layout, maxWidth * PANGO_SCALE);

    pango_layout_set_markup(layout, text.data(), text.length());
    PangoRectangle r = {};
    pango_layout_get_pixel_extents(layout, nullptr, &r);
    width = r.width;
    height = r.height;

    auto* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    absl::Cleanup clean_surface([&]{ cairo_surface_destroy(surface); });
    auto* cr = cairo_create(surface);
    absl::Cleanup clean_cr([&] { cairo_destroy(cr); });
    cairo_move_to(cr, -r.x, -r.y);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);
#if 0
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
#endif
    // TODO use SDL_CreateRGBSurfaceWithFormatFrom
    auto texture = AbyssEngine::Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::BGRA, width, height);
    texture->SetPixels(std::span(cairo_image_surface_get_data(surface), cairo_image_surface_get_height(surface) *
                cairo_image_surface_get_stride(surface)), cairo_image_surface_get_stride(surface));
    //texture->SetPixels(std::span(surface->get_data(), surface->get_height() * surface->get_stride()), surface->get_stride());
    texture->SetBlendMode(eBlendMode::Blend);
    return texture;
}

} // namespace AbyssEngine
