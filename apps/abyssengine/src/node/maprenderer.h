#ifndef ABYSS_MAPRENDERER_H
#define ABYSS_MAPRENDERER_H

#include "../systemio/interface.h"
#include "node.h"
#include "libabyss/formats/d2/palette.h"
#include <libabyss/zone/zone.h>
#include <string>
#include <memory>

namespace AbyssEngine {
class MapRenderer : public Node {
  public:
    explicit MapRenderer(LibAbyss::Zone *zone);
    ~MapRenderer() override;

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void Initialize() final;
    [[nodiscard]] std::string_view NodeType() const final { return "Map Renderer"; }
    void MouseEventCallback(const MouseEvent &event) final;

    void Compile(std::string_view paletteName);

    static void OrthoToWorld(int &x, int &y);
    static void WorldToOrtho(int &x, int &y);
    bool ShowOuterBorder = false;

  private:
    void RenderMapBorderLines(AbyssEngine::SystemIO &io) const;

    LibAbyss::Zone *_zone = nullptr;
    std::vector<Rectangle> _mapTileRects;
    std::unique_ptr<ITexture> _mapTileset;
    static void DecodeTileGraphics(LibAbyss::DT1::Tile &tile, Rectangle &tileRect, uint32_t *pixelBuffer, int textureWidth, const LibAbyss::Palette &palette);
    void GetTileSize(const LibAbyss::DT1::Tile &tile, int &width, int &height);
};
} // namespace AbyssEngine

#endif // ABYSS_MAPRENDERER_H
