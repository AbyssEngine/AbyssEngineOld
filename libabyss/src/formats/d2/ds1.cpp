#include <absl/strings/ascii.h>
#include <libabyss/formats/d2/ds1.h>
#include <libabyss/streams/streamreader.h>
#include <regex>

namespace LibAbyss {

DS1::DS1(InputStream &stream) {
    StreamReader sr(stream);

    Version = sr.ReadInt32();
    auto width = sr.ReadInt32() + 1;
    auto height = sr.ReadInt32() + 1;
    Act = (Version >= 8) ? sr.ReadInt32() : 0;

    if (Version >= 10) {
        SubstitutionType = sr.ReadInt32();
        if (SubstitutionType >= 1)
            SubstitutionGroups.emplace_back();
    }

    if (Version >= 3) {
        Files.resize(sr.ReadInt32());
        for (auto &file : Files) {
            file = absl::AsciiStrToLower(sr.ReadString());

            std::replace(file.begin(), file.end(), '\\', '/');
            file = std::regex_replace(file, std::regex("/d2/data/"), "/data/");
            file = std::regex_replace(file, std::regex("c:"), "");        // Yes they did
            file = std::regex_replace(file, std::regex(".tg1$"), ".dt1"); // Yes they did
        }
    }

    auto numFloors = 1;
    auto numShadows = 1;
    auto numSubstitutions = 0;
    auto numWalls = 0;

    if (Version >= 9 && Version <= 13)
        sr.ReadBytes(_unknown1);

    if (Version >= 4) {
        numWalls = sr.ReadInt32();
        if (Version >= 16)
            numFloors = sr.ReadInt32();
    }

    Walls.resize(numWalls);
    Shadows.resize(numShadows);
    Floors.resize(numFloors);
    Substitutions.resize(numSubstitutions);

    Resize(width, height);
    LoadLayerStreams(stream);

    if (Version >= 3)
        LoadObjects(stream);

    if ((Version >= 12) && ((SubstitutionType == 1) || (SubstitutionType == 2)))
        LoadSubstitutions(stream);

    if (Version > 14)
        LoadNPCs(stream);
}
void DS1::Resize(int width, int height) {
    Width = width;
    Height = height;

    for (auto &floor : Floors)
        floor.Resize(width, height);

    for (auto &shadow : Shadows)
        shadow.Resize(width, height);

    for (auto &wall : Walls)
        wall.Resize(width, height);

    for (auto &substitution : Substitutions)
        substitution.Resize(width, height);
}
void DS1::LoadLayerStreams(InputStream &stream) {
    const uint8_t dirLookup[25] = {0x00, 0x01, 0x02, 0x01, 0x02, 0x03, 0x03, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14};

    StreamReader sr(stream);

    auto layerStreamTypes = GetLayerStreamTypes();
    for (const auto &layerStreamType : layerStreamTypes) {
        for (auto y = 0; y < Height; y++) {
            for (auto x = 0; x < Width; x++) {
                auto dw = sr.ReadUInt32();

                switch (layerStreamType) {
                case eLayerStreamType::Wall1:
                case eLayerStreamType::Wall2:
                case eLayerStreamType::Wall3:
                case eLayerStreamType::Wall4: {
                    auto idx = (int)layerStreamType - (int)eLayerStreamType::Wall1;
                    auto &tile = Walls[idx](x, y);
                    tile.Common.Visible = (dw & 0xFF);
                    tile.Common.Sequence = (dw & 0x3F00) >> 8;
                    tile.Common._unknown1 = (dw & 0xFC000) >> 14;
                    tile.Common.Style = (dw & 0x3F00000) >> 20;
                    tile.Common._unknown2 = (dw & 0x7C000000) >> 26;
                    tile.Common.HiddenBytes = (dw & 0x80000000) >> 31;
                    break;
                }
                case eLayerStreamType::Orientation1:
                case eLayerStreamType::Orientation2:
                case eLayerStreamType::Orientation3:
                case eLayerStreamType::Orientation4: {
                    auto idx = (int)layerStreamType - (int)eLayerStreamType::Orientation1;
                    auto &tile = Walls[idx](x, y);
                    auto c = (dw & 0xFF);
                    if ((Version < 7) && (c < 25))
                        c = dirLookup[c];
                    tile.Wall.Type = c;
                    tile.Wall._zero = (dw & 0xFF00) >> 8;
                    break;
                }
                case eLayerStreamType::Floor1:
                case eLayerStreamType::Floor2: {
                    auto idx = (int)layerStreamType - (int)eLayerStreamType::Floor1;
                    auto &tile = Floors[idx](x, y);
                    tile.Common.Visible = (dw & 0xFF);
                    tile.Common.Sequence = (dw & 0x3F00) >> 8;
                    tile.Common._unknown1 = (dw & 0xFC000) >> 14;
                    tile.Common.Style = (dw & 0x3F00000) >> 20;
                    tile.Common._unknown2 = (dw & 0x7C000000) >> 26;
                    tile.Common.HiddenBytes = (dw & 0x80000000) >> 31;
                    break;
                }
                case eLayerStreamType::Shadow: {
                    auto &tile = Shadows[0](x, y);
                    tile.Common.Visible = (dw & 0xFF);
                    tile.Common.Sequence = (dw & 0x3F00) >> 8;
                    tile.Common._unknown1 = (dw & 0xFC000) >> 14;
                    tile.Common.Style = (dw & 0x3F00000) >> 20;
                    tile.Common._unknown2 = (dw & 0x7C000000) >> 26;
                    tile.Common.HiddenBytes = (dw & 0x80000000) >> 31;
                    break;
                }
                case eLayerStreamType::Substitution: {
                    Substitutions[0](x, y).Substitution.Substitution = dw;
                    break;
                }
                }
            }
        }
    }
}
std::vector<DS1::eLayerStreamType> DS1::GetLayerStreamTypes() const {
    if (Version < 4)
        return {eLayerStreamType::Wall1, eLayerStreamType::Floor1, eLayerStreamType::Orientation1, eLayerStreamType::Substitution,
                eLayerStreamType::Shadow};

    std::vector<eLayerStreamType> layerStream;
    layerStream.reserve(Walls.size() + Floors.size() + Shadows.size() + Substitutions.size());

    for (auto i = 0; i < Walls.size(); i++) {
        layerStream.push_back((eLayerStreamType)((int)eLayerStreamType::Wall1 + i));
        layerStream.push_back((eLayerStreamType)((int)eLayerStreamType::Orientation1 + i));
    }

    for (auto i = 0; i < Floors.size(); i++)
        layerStream.push_back((eLayerStreamType)((int)eLayerStreamType::Floor1 + i));

    if (!Shadows.empty())
        layerStream.push_back(eLayerStreamType::Shadow);

    if (!Substitutions.empty())
        layerStream.push_back(eLayerStreamType::Substitution);

    return layerStream;
}
void DS1::LoadObjects(InputStream &stream) {
    StreamReader sr(stream);

    auto numObjects = sr.ReadInt32();
    Objects.resize(numObjects);

    for (auto &object : Objects) {
        object.Type = sr.ReadInt32();
        object.ID = sr.ReadInt32();
        object.X = sr.ReadInt32();
        object.Y = sr.ReadInt32();
        object.Flags = sr.ReadUInt32();
    }
}
void DS1::LoadSubstitutions(InputStream &stream) {
    StreamReader sr(stream);

    if (Version >= 18)
        _unknown2 = sr.ReadUInt32();

    auto numSubstitutionGroups = sr.ReadInt32();
    SubstitutionGroups.resize(numSubstitutionGroups);

    for (auto &substitutionGroup : SubstitutionGroups) {
        substitutionGroup.TileX = sr.ReadInt32();
        substitutionGroup.TileY = sr.ReadInt32();
        substitutionGroup.WidthInTiles = sr.ReadInt32();
        substitutionGroup.HeightInTiles = sr.ReadInt32();
        substitutionGroup.Unknown = sr.ReadInt32();
    }
}
void DS1::LoadNPCs(InputStream &stream) {
    StreamReader sr(stream);

    auto numNPCs = sr.ReadInt32();
    for (auto npcIdx = 0; npcIdx < numNPCs; npcIdx++) {
        auto numPaths = sr.ReadInt32();
        auto npcX = sr.ReadInt32();
        auto npcY = sr.ReadInt32();
        auto objectIdx = -1;

        for (auto &object : Objects) {
            if (object.X != npcX || object.Y != npcY)
                continue;

            objectIdx = object.ID;
            break;
        }

        // TODO: This crashes for some reason right now
        //        if (objectIdx >= 0)
        //            LoadNPCPaths(stream, objectIdx, numPaths);
        //        else
        stream.seekg(numPaths * (Version > 15 ? 3 : 2), std::ios::cur);
    }
}
void DS1::LoadNPCPaths(InputStream &stream, int idx, int32_t paths) {
    StreamReader sr(stream);

    Objects[idx].Paths.resize(paths);

    for (auto &path : Objects[idx].Paths) {
        path.X = sr.ReadInt32();
        path.Y = sr.ReadInt32();
        path.Action == (Version > 15) ? sr.ReadInt32() : 0;
    }
}

void DS1::Layer::Resize(int width, int height) {
    _width = width;
    _height = height;
    Tiles.resize(width * height);
}

} // namespace LibAbyss
