#ifndef ABYSS_SCRIPTHOST_H
#define ABYSS_SCRIPTHOST_H

// This must be first for some reason---
#include <sol/sol.hpp>
// -------------------------------------

#include "../engine/soundeffect.h"
#include "../engine/spritefont.h"
#include "../engine/image.h"
#include "../node/button.h"
#include "../node/label.h"
#include "../node/maprenderer.h"
#include "../node/sprite.h"
#include "provider.h"
#include "../node/inputlistener.h"
#include "ttffont.h"
#include "../node/node.h"

#include <filesystem>
#include <libabyss/formats/d2/ds1.h>
#include <libabyss/zone/zone.h>

namespace AbyssEngine {

class Engine;

class ScriptHost {
  public:
    explicit ScriptHost(Engine *engine);
    std::string ExecuteString(std::string_view code);
    void ExecuteFile(std::string_view path);
    void GC();
    sol::table CreateTable() { return _lua.create_table(); }

  private:
    Engine *_engine;

    sol::state _lua;
    sol::environment _environment;
    sol::usertype<Node> _nodeType;

    std::tuple<sol::object, sol::object> LuaLoadString(std::string_view str, std::string_view chunkName);
    std::tuple<sol::object, sol::object> LuaLoadFile(std::string_view pathStr);
    sol::object LuaDoFile(std::string_view path);
    Node &LuaGetRootNode();
    template <class T, typename X>
    sol::basic_usertype<T, sol::basic_reference<false>> CreateLuaObjectType(sol::table &module, std::string_view name, X &&constructor);
    template <typename T>
    std::unique_ptr<T> InitializeTableFor(std::unique_ptr<T>);

    // -----------------------------------------------------------------------------------------------
    // Script Functions
    // -----------------------------------------------------------------------------------------------

    /// \brief Shuts down the engine
    void LuaFuncShutdown();

    /// \brief Gets a configuration value
    /// \param category The category that the variable is defined under.
    /// \param value The name of the variable who's value you want to get.
    /// \return The value of the specified category and variable.
    std::string_view LuaGetConfig(std::string_view category, std::string_view value);

    /// \brief Sets the visibility of the system cursor
    /// \param show The visibility of the system cursor.
    void LuaShowSystemCursor(bool show);

    /// \brief Logs a message
    /// \param level The log level ('debug', 'info', 'warn', 'error', 'fatal')
    /// \param message The message to log.
    void LuaLog(std::string_view level, std::string_view message);

    /// \brief Adds a loader provider
    /// \param providerType The type of provider to add ('mpq', 'casc', 'filesystem')
    void LuaAddLoaderProvider(std::string_view providerType, std::string_view providerPath);

    /// \brief Creates a palette
    /// \param paletteName The name of the palette to create.
    /// \param path The path to the palette file.
    void LuaCreatePalette(std::string_view paletteName, std::string_view path);

    /// \brief Determines if a file exists
    /// \param fileName The name of the file to check.
    /// \return True if the file exists, false otherwise.
    bool LuaFileExists(std::string_view fileName);

    /// \brief Resets the mouse button state
    void LuaResetMouseState();

    /// \brief Plays looping background music
    /// \param fileName The name of the file to play.
    void LuaPlayBackgroundMusic(std::string_view fileName);

    /// \brief Loads a text file
    /// \param filePath The path to the file to load.
    /// \return The text of the file.
    std::string LuaLoadText(std::string_view filePath);

    /// \brief Loads a .tbl file
    /// \param filePath The path to the file to load.
    /// \return The lua table. The string keys are exposed as is, the numeric keys as '#1234'
    sol::table LuaLoadTbl(std::string_view filePath);

    /// \brief Loads an image from file
    /// \param spritePath The path to the sprite file.
    /// \param paletteName The name of the palette to use for the sprite.
    /// \return The loaded image.
    std::unique_ptr<Image> LuaLoadImage(std::string_view spritePath, std::string_view paletteName);

    /// \brief Creates a sprite from the image
    /// \param image The image to be used for the sprite. Needs to be alive at least as long as the sprite is.
    /// \return The created sprite.
    std::unique_ptr<Sprite> LuaCreateSprite(Image& image);

    /// \brief Creates a button
    /// \param image The image to use for the button. Needs to be alive at least as long as the button is.
    /// \return The created button.
    std::unique_ptr<Button> LuaCreateButton(Image& image);

    /// \brief Creates a sprite font
    /// \param fontPath The path to the font file.
    /// \param paletteName The name of the palette to use for the font.
    /// \param useGlyphHeight Whether to adjust the height for difference between glyph height and frame height.
    /// \param blendMode The blend mode to use.
    /// \return The created sprite font.
    std::unique_ptr<SpriteFont> LuaCreateSpriteFont(std::string_view fontPath, std::string_view paletteName, bool
            useGlyphHeight, std::string_view blendMode);

    /// \brief Creates a TTF font
    /// \param fontName The name of the font as specified in the font file. TODO: read it automatically.
    /// \param fontPath The path to the font file.
    /// \param size The size of the font.
    /// \param hinting The hinting to use for the font ('light', 'mono', 'normal', 'none').
    std::unique_ptr<TtfFont> LuaCreateTtfFont(std::string_view name, std::string_view fontPath, int size, std::string_view hinting);

    /// \brief Creates a label
    /// \param font The font to use for the label.
    /// \return The created label.
    std::unique_ptr<Label> LuaCreateLabel(IFont &font);

    /// \brief Creates a sound effect
    /// \param fileName The name of the file to load.
    /// \return The created sound effect.
    std::unique_ptr<SoundEffect> LuaCreateSoundEffect(std::string_view fileName);

    /// \brief Creates a map renderer
    /// \param zone The zone to render.
    /// \return The created map renderer.
    std::unique_ptr<MapRenderer> LuaCreateMapRenderer(LibAbyss::Zone *zone);

    /// \brief Creates a zone (map/game area)
    /// \return The created zone.
    std::unique_ptr<LibAbyss::Zone> LuaCreateZone();

    /// \brief Loads a DS1 stamp
    /// \param fileName The name of the file to load.
    /// \return The created DS1 stamp.
    std::unique_ptr<LibAbyss::DS1> LuaLoadDS1(std::string_view fileName);

    /// \brief Creates a new input listener
    /// \return The input listener
    std::unique_ptr<InputListener> LuaCreateInputListener();

    /// \brief Sets the cursor sprite and offsets
    /// \param sprite The sprite to use for the cursor.
    /// \param offsetX The X offset of the cursor.
    /// \param offsetY The Y offset of the cursor.
    void LuaSetCursor(Sprite &sprite, int offsetX, int offsetY);

    /// \brief Plays a video
    /// \param videoPath The path to the video file.
    /// \param callback The callback to call when the video is finished.
    void LuaPlayVideo(std::string_view videoPath, const sol::safe_function &callback);

    /// \brief Plays video and audio (for videos with separate audio tracks)
    /// \param videoPath The path to the video file.
    /// \param audioPath The path to the audio file.
    /// \param callback The callback to call when the video is finished.
    void LuaPlayVideoAndAudio(std::string_view videoPath, std::string_view audioPath, const sol::safe_function &callback);

    /// \brief Converts world coordinates to orthographic coordinates
    /// \param x The world X coordinate.
    /// \param y The world Y coordinate.
    /// \return The X/Y ortho coordinates.
    std::tuple<int, int> LuaWorldToOrtho(int x, int y);

    /// \brief Converts orthographic coordinates to world coordinates
    /// \param x The ortho X coordinate.
    /// \param y The ortho Y coordinate.
    /// \return The X/Y world coordinates.
    std::tuple<int, int> LuaOrthoToWorld(int x, int y);

    /// \brief Converts UTF16 to UTF8
    /// \param str The source string.
    /// \return The string in UTF8 format.
    std::string LuaUtf16To8(const std::string& str);

    /// \brief Determines if a key was pressed
    /// \param scancode The scancode to test.
    /// \return True if the key is pressed, false otherwise.
    bool LuaIsKeyPressed(uint16_t scancode);

    /// \brief Creates a new empty node
    /// \return The new node
    std::unique_ptr<Node> LuaCreateNode();
    void DefineScanCodes(sol::table module);
};

} // namespace AbyssEngine

#endif // ABYSS_SCRIPTHOST_H
