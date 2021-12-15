---@meta

---@class Sprite
Sprite = {

    ---Description missing for LuaGetBlendMode in sprite.h:40
    ---@type string
    blendMode,

    ---Description missing for GetIsBottomOrigin in sprite.h:43
    ---@type boolean
    bottomOrigin,

    ---Description missing for LuaGetPlayMode in sprite.h:45
    ---@type string
    playMode,
}

function Sprite:setCellSize() end
function Sprite:getCellSize() end
function Sprite:onMouseButtonDown() end
function Sprite:onMouseButtonUp() end
