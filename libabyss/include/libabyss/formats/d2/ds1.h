#ifndef ABYSS_DS1_H
#define ABYSS_DS1_H

#include <libabyss/streams/inputstream.h>
#include <string>
#include <vector>

namespace LibAbyss {
class DS1 {
  public:
    enum class eLayerGroupType { Floor, Wall, Shadow, Substitution };
    enum class eLayerStreamType {
        Wall1,
        Wall2,
        Wall3,
        Wall4,
        Orientation1,
        Orientation2,
        Orientation3,
        Orientation4,
        Floor1,
        Floor2,
        Shadow,
        Substitution
    };

    struct Tile {
        struct {
            uint8_t Prop1;
            uint8_t Sequence;
            uint8_t _unknown1;
            uint8_t Style;
            uint8_t _unknown2;
            uint8_t HiddenBytes;
            uint8_t RandomIndex;
            int YAdjust;
        } Common;
        struct {
            uint32_t Substitution;
        } Substitution;
        struct {
            uint8_t Type;
            uint8_t _zero;
        } Wall;
        struct {
            bool Animated;
        } FloorShadow;
    };

    struct Path {
        int32_t X;
        int32_t Y;
        int32_t Action;
    };

    struct Object {
        int32_t Type;
        int32_t ID;
        int32_t X;
        int32_t Y;
        uint32_t Flags;
        std::vector<Path> Paths;
    };

    struct SubstitutionGroup {
        int32_t TileX;
        int32_t TileY;
        int32_t WidthInTiles;
        int32_t HeightInTiles;
        int32_t Unknown;
    };

    struct Layer {
        std::vector<Tile> Tiles;
        void Resize(int width, int height);
        int _width;
        int _height;
        Tile &operator()(int x, int y) { return Tiles[x + (y * _width)]; }
        const Tile &operator()(int x, int y) const { return Tiles[x + (y * _width)]; }
    };

    explicit DS1(InputStream &stream);
    void Resize(int width, int height);
    [[nodiscard]] std::vector<eLayerStreamType> GetLayerStreamTypes() const;

    std::vector<std::string> Files;
    std::vector<Object> Objects;
    std::vector<SubstitutionGroup> SubstitutionGroups;
    std::vector<Layer> Floors;
    std::vector<Layer> Walls;
    std::vector<Layer> Shadows;
    std::vector<Layer> Substitutions;

    uint32_t Version;
    int32_t Act;              // Act from 1 to 5. Tells which act table to use for objects list.
    int32_t SubstitutionType; // Aka layer type. 0 if no layer, otherwise type 1 or 2.
    uint8_t _unknown1[8];
    uint32_t _unknown2;
    int Width;
    int Height;

  private:
    void LoadLayerStreams(InputStream &stream);

    void LoadObjects(InputStream &stream);
    void LoadSubstitutions(InputStream &stream);
    void LoadNPCs(InputStream &stream);
    void LoadNPCPaths(InputStream &stream, int idx, int32_t paths);
};
} // namespace LibAbyss

#endif // ABYSS_DS1_H
