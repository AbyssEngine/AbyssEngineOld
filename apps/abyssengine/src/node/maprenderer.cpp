#include "maprenderer.h"
#include "../engine/engine.h"
#include <cmath>

AbyssEngine::MapRenderer::MapRenderer(LibAbyss::Zone *zone) : _zone(zone) {}

void AbyssEngine::MapRenderer::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void AbyssEngine::MapRenderer::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();

    if (ShowOuterBorder)
        RenderMapBorderLines(io);

    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::MapRenderer::RenderMapBorderLines(AbyssEngine::SystemIO &io) const {
    auto &renderer = Engine::Get()->GetSystemIO();

    int x1;
    int y1;
    int x2;
    int y2;

    // TL->TR
    x1 = 0;
    y1 = 0;
    x2 = _zone->WidthInTiles;
    y2 = 0;
    WorldToOrtho(x1, y1);
    WorldToOrtho(x2, y2);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);

    // TR->BR
    x1 = _zone->WidthInTiles;
    y1 = _zone->HeightInTiles;
    WorldToOrtho(x1, y1);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);

    // BR->BL
    x2 = 0;
    y2 = _zone->HeightInTiles;
    WorldToOrtho(x2, y2);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);

    // BL->TL
    x1 = 0;
    y1 = 0;
    WorldToOrtho(x1, y1);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);
}

void AbyssEngine::MapRenderer::Initialize() { Node::Initialize(); }

void AbyssEngine::MapRenderer::MouseEventCallback(const AbyssEngine::MouseEvent &event) { Node::MouseEventCallback(event); }

void AbyssEngine::MapRenderer::OrthoToWorld(int &x, int &y) {
    auto x2 = (float)x;
    auto y2 = (float)y;
    auto worldX = (x2 / 80.f + y2 / 40.f) / 2.f;
    auto worldY = (y2 / 40.f - x2 / 80.f) / 2.f;

    x = (int)worldX;
    y = (int)worldY;
}
void AbyssEngine::MapRenderer::WorldToOrtho(int &x, int &y) {
    auto x2 = (float)x;
    auto y2 = (float)y;
    auto orthoX = (x2 - y2) * 40.f;
    auto orthoY = (x2 + y2) * 20.f;

    x = (int)orthoX;
    y = (int)orthoY;
}
