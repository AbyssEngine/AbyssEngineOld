#include "maprenderer.h"
AbyssEngine::MapRenderer::MapRenderer() {}

void AbyssEngine::MapRenderer::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void AbyssEngine::MapRenderer::RenderCallback(int offsetX, int offsetY) {
    //
    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::MapRenderer::Initialize() { Node::Initialize(); }

void AbyssEngine::MapRenderer::MouseEventCallback(const AbyssEngine::MouseEvent &event) { Node::MouseEventCallback(event); }
