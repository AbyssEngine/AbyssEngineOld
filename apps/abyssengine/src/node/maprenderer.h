#ifndef ABYSS_MAPRENDERER_H
#define ABYSS_MAPRENDERER_H

#include "node.h"
#include <libabyss/zone/zone.h>

namespace AbyssEngine {
class MapRenderer : public Node  {
  public:
    MapRenderer();

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void Initialize() final;
    [[nodiscard]] std::string_view NodeType() const final { return "Map Renderer"; }
    void MouseEventCallback(const MouseEvent &event) final;

};
} // namespace AbyssEngine

#endif // ABYSS_MAPRENDERER_H
