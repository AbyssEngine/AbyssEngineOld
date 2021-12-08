---@meta
-- version: 0.1

---@class abyss
abyss = {}

---Adds a loader provider.
---@param providerType string # The type of provider.
---Valid values are:
--- - "mpq"
--- - "casc"
--- - "filesystem"
---@param providerPath string # The path to the provider file (or folder for filesystem)
function abyss.addLoaderProvider(providerType, providerPath) end


---Determines if a file exists or not.
---@param filePath string # The path to the file
---@return boolean # True if the file exists. False otherwise.
function abyss.fileExists(filePath) end


---Creates a sprite.
---@param filePath string # The path to the file
---@param paletteName string # The name of the palette to use
---@return Sprite value # The sprite
function abyss.createSprite(filePath, paletteName) end


---Gets a configuration value
---@param category string     # The category that the variable is defined under.
---@param variableName string # The name of the variable who's value you want to get.
---@return string value       # The value of the specified category and variable.
function abyss.getConfig(category, variableName) end



---Logs a message
---@param level string   # The log level.
---Valid values are:
--- - "debug"
--- - "info"
--- - "warn"
--- - "error"
--- - "fatal"
---@param message string # The message to log.
function abyss.log(level, message) end
