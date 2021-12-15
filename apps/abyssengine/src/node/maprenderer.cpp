#include "maprenderer.h"
#include "../engine/engine.h"
#include <cmath>
#include <libabyss/common/tiletype.h>

AbyssEngine::MapRenderer::MapRenderer(LibAbyss::Zone *zone) : _zone(zone) {}

AbyssEngine::MapRenderer::~MapRenderer() {}

void AbyssEngine::MapRenderer::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void AbyssEngine::MapRenderer::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();

    // ---------------------------------------------------------------------------------------------------------------------------
    // Pass 1
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

                const auto ett = (TileType)tile.Type;
                if (!(ett == TileType::LowerWallsEquivalentToLeftWall || ett == TileType::LowerWallsEquivalentToRightWall ||
                    ett == TileType::LowerWallsEquivalentToRightLeftNorthCornerWall || ett == TileType::LowerWallsEquivalentToSouthCornerWall))
                    continue;

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }

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
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }

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
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y - 80, .Width = tileRect.Width, .Height = tileRect.Height};
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
    // Pass 2
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

                if ((tile.Type < (int)TileType::LeftWall) || (tile.Type > (int)TileType::Tree))
                    continue;

                if ((tile.Type == (int)TileType::Shadow) || (tile.Type == (int)TileType::SpecialTile1) || (tile.Type == (int)TileType::SpecialTile2))
                    continue;

                const auto &tileRect = _mapTileRects[tileIndex];
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y - tileRect.Height + 80, .Width = tileRect.Width, .Height = tileRect.Height};

                _mapTileset->Render(tileRect, destRect);
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
                Rectangle destRect = {.X = dx - 80 + X, .Y = dy + Y - tileRect.Height + 80, .Width = tileRect.Width, .Height = tileRect.Height};
                _mapTileset->Render(tileRect, destRect);
            }
        }
    }



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
    x2 = _zone->WidthInTiles + 1;
    y2 = 0;
    WorldToOrtho(x1, y1);
    WorldToOrtho(x2, y2);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);

    // TR->BR
    x1 = _zone->WidthInTiles + 1;
    y1 = _zone->HeightInTiles + 1;
    WorldToOrtho(x1, y1);
    io.DrawLine(X + x1, Y + y1, X + x2, Y + y2, 0x00, 0xFF, 0x00);

    // BR->BL
    x2 = 0;
    y2 = _zone->HeightInTiles + 1;
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
void AbyssEngine::MapRenderer::Compile(std::string_view paletteName) {
    const auto &palette = Engine::Get()->GetPalette(paletteName);
    _mapTileRects.clear();

    const auto maxTextureWidth = 8128;
    int nextRowY = 0;
    int curX = 0;
    int curY = 0;
    int textureWidth = 0;
    int textureHeight = 0;

    for (const auto &tile : _zone->Tiles) {
        int tileMinX = 999999;
        int tileMinY = 999999;
        int tileMaxX = -999999;
        int tileMaxY = -999999;

        int tileWidth = tile.Width;
        int tileHeight = tile.Height;

        if (!tile.Blocks.empty()) {
            for (const auto &block : tile.Blocks) {
                if (block.X < tileMinX)
                    tileMinX = block.X;
                if (block.Y < tileMinY)
                    tileMinY = block.Y;
                if (block.X + 32 > tileMaxX)
                    tileMaxX = block.X + 32;
                if (block.Y + 32 > tileMaxY)
                    tileMaxY = block.Y + 32;
            }


            tileWidth = tileMaxX - tileMinX;
            tileHeight = tileMaxY - tileMinY;
        }

        if (tileWidth < 0) {
            tileWidth = 0;
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
        DecodeTileGraphics(tile, tileRect, pixels.data(), textureWidth, palette);
    }

    _mapTileset = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::Static, textureWidth, textureHeight);
    _mapTileset->SetPixels(pixels);
    _mapTileset->SetBlendMode(eBlendMode::Blend);
}
void AbyssEngine::MapRenderer::DecodeTileGraphics(LibAbyss::DT1::Tile &tile, AbyssEngine::Rectangle &tileRect, uint32_t *pixelBuffer,
                                                  int textureWidth, const LibAbyss::Palette &palette) {
    int minY = 0;
    for (const auto& block : tile.Blocks) {
        if (block.Y >= minY)
            continue;
        minY = block.Y;
    }

    auto adjust = (minY < 0) ? -minY : 0;


    //
    for (const auto &block : tile.Blocks) {
        switch (block.Format) {
        case LibAbyss::DT1::Tile::Block::eBlockFormat::RLE: {
            int32_t blockX = block.X + tileRect.X;
            int32_t blockY = block.Y + tileRect.Y + adjust;
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
            int32_t blockY = block.Y + tileRect.Y + adjust;
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
