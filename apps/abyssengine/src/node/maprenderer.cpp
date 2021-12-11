#include "maprenderer.h"
#include "../engine/engine.h"
#include <libabyss/formats/d2/dt1.h>
#include <libabyss/formats/d2/ds1.h>

AbyssEngine::MapRenderer::MapRenderer() {
    auto stream = Engine::Get()->GetLoader().Load("/data/global/tiles/ACT1/TOWN/townW1.ds1");
    auto testDs1 = LibAbyss::DS1(stream);
    int x = 0;
}

void AbyssEngine::MapRenderer::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void AbyssEngine::MapRenderer::RenderCallback(int offsetX, int offsetY) {
    //
    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::MapRenderer::Initialize() { Node::Initialize(); }

void AbyssEngine::MapRenderer::MouseEventCallback(const AbyssEngine::MouseEvent &event) { Node::MouseEventCallback(event); }
