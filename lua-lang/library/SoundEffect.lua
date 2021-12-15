---@meta

---@class SoundEffect
SoundEffect = {

    ---Description missing for GetVolume in soundeffect.h:17
    ---@type number
    volume,

    ---Description missing for GetLoop in soundeffect.h:19
    ---@type boolean
    loop,
}

function SoundEffect:play() end
function SoundEffect:stop() end
function SoundEffect:pause() end
function SoundEffect:isPlaying() end
function SoundEffect:isPaused() end
