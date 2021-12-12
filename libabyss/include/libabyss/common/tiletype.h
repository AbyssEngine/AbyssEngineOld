#ifndef ABYSS_TILETYPE_H
#define ABYSS_TILETYPE_H

enum class TileType {
    Floor,
    LeftWall,
    RightWall,
    RightPartOfNorthCornerWall,
    LeftPartOfNorthCornerWall,
    LeftEndWall,
    RightEndWall,
    SouthCornerWall,
    LeftWallWithDoor,
    RightWallWithDoor,
    SpecialTile1,
    SpecialTile2,
    PillarsColumnsAndStandaloneObjects,
    Shadow,
    Tree,
    Roof,
    LowerWallsEquivalentToLeftWall,
    LowerWallsEquivalentToRightWall,
    LowerWallsEquivalentToRightLeftNorthCornerWall,
    LowerWallsEquivalentToSouthCornerWall,
};

#endif // ABYSS_TILETYPE_H
