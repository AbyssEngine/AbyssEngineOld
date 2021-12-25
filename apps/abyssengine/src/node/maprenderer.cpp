#include "maprenderer.h"
#include "../engine/engine.h"
#include <cmath>
#include <libabyss/common/tiletype.h>

namespace AbyssEngine {
MapRenderer::MapRenderer(LibAbyss::Zone *zone) : _zone(zone) {}

MapRenderer::~MapRenderer() {}

void MapRenderer::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void MapRenderer::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();

    //    // Lower Walls (below the floor
    for (int y = 0; y < _zone->HeightInTiles; y++) {
        for (int x = 0; x < _zone->WidthInTiles; x++) {
            int dx = x;
            int dy = y;
            WorldToOrtho(dx, dy);
            for (const auto &wall : _zone->Walls) {
                const auto &tileIndex = wall[x + (y * _zone->WidthInTiles)];
                if (tileIndex < 0)
                    continue;
                const auto &tile = _zone->Tiles[tileIndex];

                if (tile.GetTileType() != LibAbyss::DT1::GeneralTileType::LowerWall)
                    continue;

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y + tile.YAdjust, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }

    // Floors
    for (int y = 0; y < _zone->HeightInTiles; y++) {
        for (int x = 0; x < _zone->WidthInTiles; x++) {
            int dx = x;
            int dy = y;
            WorldToOrtho(dx, dy);
            for (const auto &floor : _zone->Floors) {
                const auto &tileIndex = floor[x + (y * _zone->WidthInTiles)];
                if (tileIndex < 0)
                    continue;
                const auto &tile = _zone->Tiles[tileIndex];

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y + tile.YAdjust, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }
    //
    //    // Shadows
    for (int y = 0; y < _zone->HeightInTiles; y++) {
        for (int x = 0; x < _zone->WidthInTiles; x++) {
            int dx = x;
            int dy = y;
            WorldToOrtho(dx, dy);
            for (const auto &shadow : _zone->Shadows) {
                const auto &tileIndex = shadow[x + (y * _zone->WidthInTiles)];
                if (tileIndex < 0)
                    continue;
                const auto &tile = _zone->Tiles[tileIndex];
                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y + tile.YAdjust, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }

    // ---------------------------------------------------------------------------------------------------------------------------
    // Debug Stuff
    // ---------------------------------------------------------------------------------------------------------------------------

    if (ShowOuterBorder)
        RenderMapBorderLines(io);
    // ---------------------------------------------------------------------------------------------------------------------------

    // Walls
    for (int y = 0; y < _zone->HeightInTiles; y++) {
        for (int x = 0; x < _zone->WidthInTiles; x++) {
            int dx = x;
            int dy = y;
            WorldToOrtho(dx, dy);
            for (const auto &wall : _zone->Walls) {
                const auto &tileIndex = wall[x + (y * _zone->WidthInTiles)];
                if (tileIndex < 0)
                    continue;
                const auto &tile = _zone->Tiles[tileIndex];

                if (tile.GetTileType() != LibAbyss::DT1::GeneralTileType::NormalWall)
                    continue;

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = (dy + Y) + tile.YAdjust, .Width = tileRect.Width, .Height = tileRect.Height};

                _mapTileset->Render(tileRect, destRect);

                if (tile.AltTile >= 0) {
                    const auto &tr = _mapTileRects[tile.AltTile];
                    Rectangle dr = {.X = dx - 80 + X, .Y = (dy + Y) + _zone->Tiles[tile.AltTile].YAdjust, .Width = tr.Width, .Height = tr.Height};
                    _mapTileset->Render(tr, dr);
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------------------------------------
    // Pass 3
    // ---------------------------------------------------------------------------------------------------------------------------
    for (int y = 0; y < _zone->HeightInTiles; y++) {
        for (int x = 0; x < _zone->WidthInTiles; x++) {
            int dx = x;
            int dy = y;
            WorldToOrtho(dx, dy);
            for (const auto &wall : _zone->Walls) {
                const auto &tileIndex = wall[x + (y * _zone->WidthInTiles)];
                if (tileIndex < 0)
                    continue;
                const auto &tile = _zone->Tiles[tileIndex];

                if (tile.Type != (int)TileType::Roof)
                    continue;

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y + tile.YAdjust, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }

    Node::RenderCallback(offsetX, offsetY);
}

void MapRenderer::RenderMapBorderLines(SystemIO &io) const {
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

void MapRenderer::Initialize() { Node::Initialize(); }

void MapRenderer::MouseEventCallback(const MouseEvent &event) { Node::MouseEventCallback(event); }

void MapRenderer::OrthoToWorld(int &x, int &y) {
    auto x2 = (float)x;
    auto y2 = (float)y;
    auto worldX = (x2 / 80.f + y2 / 40.f) / 2.f;
    auto worldY = (y2 / 40.f - x2 / 80.f) / 2.f;

    x = (int)worldX;
    y = (int)worldY;
}
void MapRenderer::WorldToOrtho(int &x, int &y) {
    auto x2 = (float)x;
    auto y2 = (float)y;
    auto orthoX = (x2 - y2) * 80.f;
    auto orthoY = (x2 + y2) * 40.f;

    x = (int)orthoX;
    y = (int)orthoY;
}
void MapRenderer::Compile(std::string_view paletteName) {
    const auto &palette = Engine::Get()->GetPalette(paletteName);
    _mapTileRects.clear();

    const auto maxTextureWidth = 8192;
    int nextRowY = 0;
    int curX = 0;
    int curY = 0;
    int textureWidth = 0;
    int textureHeight = 0;

    for (int i = 0; i < _zone->Tiles.size(); i++) {
        const auto &tile = _zone->Tiles[i];

        if (!tile.InUse) {
            _mapTileRects.push_back({});
            continue;
        }

        int tileWidth, tileHeight;
        GetTileSize(tile, tileWidth, tileHeight);

        if (tileWidth == 0 || tileHeight == 0) {
            _mapTileRects.push_back({});
            continue;
        }

        if (curX + tileWidth >= maxTextureWidth) {
            curX = 0;
            curY = nextRowY;
        }

        Rectangle rect = {.X = curX, .Y = curY, .Width = tileWidth, .Height = tileHeight};

        curX += tileWidth;
        if (curY + tileHeight > nextRowY)
            nextRowY = curY + tileHeight;

        _mapTileRects.push_back(rect);

        if (textureWidth < curX + tileWidth)
            textureWidth = curX + tileWidth;

        if (textureHeight < curY + tileHeight)
            textureHeight = curY + tileHeight;
    }

    std::vector<uint32_t> pixels;
    pixels.resize(textureWidth * textureHeight);

    memset(pixels.data(), 0, pixels.size() * sizeof(uint32_t));

    for (auto i = 0; i < _zone->Tiles.size(); i++) {
        auto &tileRect = _mapTileRects[i];
        auto &tile = _zone->Tiles[i];
        if (tileRect.Width == 0 || tileRect.Height == 0) {
            continue;
        }

        //        const uint32_t color = 0xFF000000 | (0xFF << (( i & 1) * 8));
        //
        //        for (int y = 0; y < tileRect.Height; y++) {
        //            for (int x = 0; x < tileRect.Width; x++) {
        //
        //                auto offset = (x + tileRect.X + ((y + tileRect.Y) * textureWidth));
        //                pixels[offset] = color;
        //            }
        //        }

        DecodeTileGraphics(tile, tileRect, pixels.data(), textureWidth, palette);
    }

    _mapTileset = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::Static, textureWidth, textureHeight);
    _mapTileset->SetPixels(pixels);
    _mapTileset->SetBlendMode(eBlendMode::Blend);
    //    _mapTileset->SaveAsBMP("/Users/essial/MapTileSet.bmp");
}
void MapRenderer::DecodeTileGraphics(LibAbyss::DT1::Tile &tile, Rectangle &tileRect, uint32_t *pixelBuffer, int textureWidth,
                                     const LibAbyss::Palette &palette) {
    int32_t offsetY;

    switch (tile.GetTileType()) {
    case LibAbyss::DT1::GeneralTileType::Floor:
    case LibAbyss::DT1::GeneralTileType::Roof:
        // Zero point is 0
        offsetY = 0;
        break;
    case LibAbyss::DT1::GeneralTileType::LowerWall:
        // Zero point is -96
        offsetY = 96;
        break;
    default:
        // Zero point is the bottom
        offsetY = -tile.Height;
        break;
    }

    //    offsetY = (tile.Height < 0) ? -tile.Height : 0;

    for (const auto &block : tile.Blocks) {
        switch (block.Format) {
        case LibAbyss::DT1::Tile::Block::eBlockFormat::RLE: {
            int32_t blockX = block.X + tileRect.X;
            int32_t blockY = block.Y + tileRect.Y + offsetY;
            int32_t x = 0;
            int32_t y = 0;
            int32_t idx = 0;
            int32_t length = block.Length;

            while (length > 0) {
                uint8_t b1 = block.EncodedBytes[idx];
                uint8_t b2 = block.EncodedBytes[idx + 1];
                idx += 2;
                length -= 2;

                if ((b1 | b2) == 0) {
                    x = 0;
                    y++;

                    continue;
                }

                x += b1;
                length -= b2;

                while (b2 > 0) {
                    int offset = ((blockY + y) * textureWidth) + (blockX + x);

                    pixelBuffer[offset] = (uint32_t)palette.BasePalette[block.EncodedBytes[idx]];
                    idx++;
                    x++;
                    b2--;
                }
            }
            break;
        }
        case LibAbyss::DT1::Tile::Block::eBlockFormat::Isometric: {
            int32_t xJump[] = {14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14};
            int32_t nbPix[] = {4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4};
            int32_t blockX = block.X + tileRect.X;
            int32_t blockY = block.Y + tileRect.Y + offsetY;
            int32_t length = block.Length;
            int32_t x = 0;
            int32_t y = 0;
            int32_t idx = 0;

            while (length > 0) {
                x = xJump[y];
                int32_t n = nbPix[y];
                length -= n;

                while (n > 0) {
                    uint32_t offset = ((blockY + y) * textureWidth) + (blockX + x);
                    pixelBuffer[offset] = (uint32_t)palette.BasePalette[block.EncodedBytes[idx]];
                    x++;
                    n--;
                    idx++;
                }
                y++;
            }
            break;
        }
        }
    }
}
void MapRenderer::GetTileSize(const LibAbyss::DT1::Tile &tile, int &width, int &height) {
    width = tile.Width;
    height = tile.Height < 0 ? -tile.Height : tile.Height;

    if (tile.Blocks.empty() || tile.Width == 0 || tile.Height == 0)
        return;

    const int32_t offset_y = (tile.Height < 0) ? -tile.Height : 0;

    //    int32_t minY = INT32_MAX;
    int32_t maxY = INT32_MIN;
    //    int32_t minX = INT32_MAX;
    int32_t maxX = INT32_MIN;

    for (const auto &block : tile.Blocks) {
        //        if (block.X < minX)
        //            minX = block.X;

        //        if (block.Y + offset_y < minY)
        //            minY = block.Y + offset_y;

        if (block.X + 32 > maxX)
            maxX = block.X + 32;

        if (block.Y + 32 + offset_y > maxY)
            maxY = block.Y + 32 + offset_y;
    }

    width = maxX;
    height = maxY;

    auto actualTileWidth = tile.Width;
    auto actualTileHeight = tile.Height;

    if (actualTileHeight < 0)
        actualTileHeight = -actualTileHeight;

    if (width < actualTileWidth)
        width = actualTileWidth;

    if (height < actualTileHeight)
        height = actualTileHeight;
}
} // namespace AbyssEngine
