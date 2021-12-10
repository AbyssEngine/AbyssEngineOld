---@meta
-- version: 0.1

---@class abyss
abyss = {}

---Adds a loader provider.
---@param providerType string @ The type of provider.
---Valid values are:
--- - "mpq"
--- - "casc"
--- - "filesystem"
---@param providerPath string @ The path to the provider file (or folder for filesystem)
function abyss.addLoaderProvider(providerType, providerPath) end

---Creates a button.
---@param spriteFont SpriteFont @ The sprite font to use for the text.
---@param sprite Sprite         @ This sprite to use for the button frames.
---@return Button button
function abyss.createButton(spriteFont, sprite) end

---Creates a label
---@param spriteFont SpriteFont @ The sprite font to use for the text.
---@return Label label
function abyss.createLabel(spriteFont) end

---Creates a palette
---@param paletteName string @ The name of the palette
---@param path string        @ The path to the palette file
function abyss.createPalette(paletteName, path) end

---Creates a sound effect
---@param path string @ The path to the sound effect file
---@return SoundEffect soundEffect
function abyss.createSoundEffect(fileName) end

---Creates a sprite.
---@param filePath string    @ The path to the file
---@param paletteName string @ The name of the palette to use
---@return Sprite value      @ The sprite
function abyss.createSprite(filePath, paletteName) end

---Creates a sprite font.
---@param filePath string    @ The path to the sprite font file
---@param paletteName string @ The name of the palette to use
---@return SpriteFont spriteFont
function abyss.createSpriteFont(fontPath, paletteName) end

---Creates a string from a file
---@param filePath string @ The path to the file
---@return string value   @ The string
function abyss.createString(filePath) end

---Determines if a file exists or not.
---@param filePath string @ The path to the file
---@return boolean        @ True if the file exists. False otherwise.
function abyss.fileExists(filePath) end

---Gets a configuration value
---@param category string     @ The category that the variable is defined under.
---@param variableName string @ The name of the variable who's value you want to get.
---@return string value       @ The value of the specified category and variable.
function abyss.getConfig(category, variableName) end

---Gets the root node
---@return Node node @ The root node
function abyss.getRootNode() end

---Logs a message
---@param level string   @ The log level.
---Valid values are:
--- - "debug"
--- - "info"
--- - "warn"
--- - "error"
--- - "fatal"
---@param message string @ The message to log.
function abyss.log(level, message) end

---Plays the specified sound as looping background music.
---@param fileName string @ The path to the sound effect file
function abyss.playBackgroundMusic(fileName) end

---Plays the specified video
---@param videoPath string @ The path to the video file
---@param onFinishedCallback function @ The callback to call when the video finishes playing.
function abyss.playVideo(videoPath, onFinishedCallback) end

---Resets the mouse state.
---This is usefull to prevent accidental button clicks on scene changes.
function abyss.resetMouseState() end

---Sets the cursor to the specified sprite.
---@param sprite Sprite  @ The sprite to use as the cursor.
---@param offsetX number @ The x offset of the cursor from the origin.
---@param offsetY number @ The y offset of the cursor from the origin.
function abyss.setCursor(sprite, offsetX, offsetY) end

---Shows or hides the cursor.
---@param show boolean @ True to show the cursor. False to hide it.
function abyss.showSystemCursor(show) end

---Shuts down the engine
function abyss.shutdown() end
