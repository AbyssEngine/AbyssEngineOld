---@meta

---@class Sprite
Sprite = {

    ---Description missing for LuaGetBlendMode in sprite.h:28
    ---@type string
    blendMode,

    ---Description missing for GetIsBottomOrigin in sprite.h:31
    ---@type boolean
    bottomOrigin,

    ---Description missing for LuaGetPlayMode in sprite.h:33
    ---@type string
    playMode,

    ---Description missing for GetCurrentFrameIndex in sprite.h:36
    ---@type unumber32_t
    currentFrameIndex,
}

function Sprite:setCellSize() end
function Sprite:getCellSize() end
function Sprite:onMouseButtonDown() end
function Sprite:onMouseButtonUp() end
