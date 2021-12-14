#ifndef ABYSS_MAPRENDERER_H
#define ABYSS_MAPRENDERER_H

#include "node.h"
#include "../systemio/interface.h"
#include <libabyss/zone/zone.h>

namespace AbyssEngine {
class MapRenderer : public Node  {
  public:
    explicit MapRenderer(LibAbyss::Zone *zone);

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void Initialize() final;
    [[nodiscard]] std::string_view NodeType() const final { return "Map Renderer"; }
    void MouseEventCallback(const MouseEvent &event) final;
    static void OrthoToWorld(int &x, int &y);
    static void WorldToOrtho(int &x, int &y);
    bool ShowOuterBorder = false;

  private:
    LibAbyss::Zone *_zone;
    void RenderMapBorderLines(AbyssEngine::SystemIO &io) const;
};
} // namespace AbyssEngine

#endif // ABYSS_MAPRENDERER_H
