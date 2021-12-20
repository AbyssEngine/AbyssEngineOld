#include "scripthost.h"
#include "../node/d2rsprite.h"
#include "../node/dc6sprite.h"
#include "../node/inputlistener.h"
#include "cascprovider.h"
#include "engine.h"
#include "filesystemprovider.h"
#include "font.h"
#include "image.h"
#include "mpqprovider.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <codecvt>
#include <filesystem>
#include <libabyss/common/levelpreset.h>
#include <libabyss/common/leveltype.h>
#include <libabyss/formats/d2/dt1.h>
#include <locale>
#include <memory>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

static int my_exception_handler(lua_State *L, sol::optional<const std::exception &> maybe_exception, sol::string_view description) {
    // L is the lua state, which you can wrap in a state_view if necessary
    // maybe_exception will contain exception, if it exists
    // description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
    std::cout << "An exception occurred in a function, here's what it says ";
    if (maybe_exception) {
        std::cout << "(straight from the exception): ";
        const std::exception &ex = *maybe_exception;
        std::cout << ex.what() << std::endl;
    } else {
        std::cout << "(from the description parameter): ";
        std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
        std::cout << std::endl;
    }

    // you must push 1 element onto the stack to be
    // transported through as the error object in Lua
    // note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
    // so we push a single string (in our case, the description of the error)
    return sol::stack::push(L, description);
}

AbyssEngine::ScriptHost::ScriptHost(Engine *engine) : _engine(engine), _lua() {
    _lua.stop_gc();
    _lua.set_exception_handler(&my_exception_handler);
    _lua.open_libraries();

    _environment = sol::environment(_lua, sol::create, _lua.globals());
    sol::table module = _lua.create_table("abyss");

    // Overload loading functions ---------------------------------------------------------------------------------------------------------

    _environment.set_function("loadstring", &ScriptHost::LuaLoadString, this);
    _environment.set_function("loadfile", &ScriptHost::LuaLoadFile, this);
    _environment.set_function("dofile", &ScriptHost::LuaDoFile, this);
    _environment.set_function("require", &ScriptHost::LuaLoadFile, this);

    // Engine Functions -------------------------------------------------------------------------------------------------------------------

    module.set_function("addLoaderProvider", &ScriptHost::LuaAddLoaderProvider, this);
    module.set_function("createButton", &ScriptHost::LuaCreateButton, this);
    module.set_function("createInputListener", &ScriptHost::LuaCreateInputListener, this);
    module.set_function("createLabel", &ScriptHost::LuaCreateLabel, this);
    module.set_function("createMapRenderer", &ScriptHost::LuaCreateMapRenderer, this);
    module.set_function("createPalette", &ScriptHost::LuaCreatePalette, this);
    module.set_function("createSoundEffect", &ScriptHost::LuaCreateSoundEffect, this);
    module.set_function("createSprite", &ScriptHost::LuaCreateSprite, this);
    module.set_function("createSpriteFont", &ScriptHost::LuaCreateSpriteFont, this);
    module.set_function("createTtfFont", &ScriptHost::LuaCreateTtfFont, this);
    module.set_function("isKeyPressed", &ScriptHost::LuaIsKeyPressed, this);
    module.set_function("loadDS1", &ScriptHost::LuaLoadDS1, this);
    module.set_function("loadImage", &ScriptHost::LuaLoadImage, this);
    module.set_function("loadString", &ScriptHost::LuaLoadText, this);
    module.set_function("createZone", &ScriptHost::LuaCreateZone, this);
    module.set_function("fileExists", &ScriptHost::LuaFileExists, this);
    module.set_function("getConfig", &ScriptHost::LuaGetConfig, this);
    module.set_function("getRootNode", &ScriptHost::LuaGetRootNode, this);
    module.set_function("log", &ScriptHost::LuaLog, this);
    module.set_function("orthoToWorld", &ScriptHost::LuaOrthoToWorld, this);
    module.set_function("playBackgroundMusic", &ScriptHost::LuaPlayBackgroundMusic, this);
    module.set_function("playVideo", &ScriptHost::LuaPlayVideo, this);
    module.set_function("playVideoAndAudio", &ScriptHost::LuaPlayVideoAndAudio, this);
    module.set_function("resetMouseState", &ScriptHost::LuaResetMouseState, this);
    module.set_function("setCursor", &ScriptHost::LuaSetCursor, this);
    module.set_function("showSystemCursor", &ScriptHost::LuaShowSystemCursor, this);
    module.set_function("shutdown", &ScriptHost::LuaFuncShutdown, this);
    module.set_function("utf16To8", &ScriptHost::LuaUtf16To8, this);
    module.set_function("worldToOrtho", &ScriptHost::LuaWorldToOrtho, this);

    module["scanCodes"] = _lua.create_table_with(
        "UNKNOWN", 0, "A", 4, "B", 5, "C", 6, "D", 7, "E", 8, "F", 9, "G", 10, "H", 11, "I", 12, "J", 13, "K", 14, "L", 15, "M", 16, "N", 17, "O", 18,
        "P", 19, "Q", 20, "R", 21, "S", 22, "T", 23, "U", 24, "V", 25, "W", 26, "X", 27, "Y", 28, "Z", 29, "NUM_1", 30, "NUM_2", 31, "NUM_3", 32,
        "NUM_4", 33, "NUM_5", 34, "NUM_6", 35, "NUM_7", 36, "NUM_8", 37, "NUM_9", 38, "NUM_0", 39, "RETURN", 40, "ESCAPE", 41, "BACKSPACE", 42, "TAB",
        43, "SPACE", 44, "MINUS", 45, "EQUALS", 46, "LEFTBRACKET", 47, "RIGHTBRACKET", 48, "BACKSLASH", 49, "NONUSHASH", 50, "SEMICOLON", 51,
        "APOSTROPHE", 52, "GRAVE", 53, "COMMA", 54, "PERIOD", 55, "SLASH", 56, "CAPSLOCK", 57, "F1", 58, "F2", 59, "F3", 60, "F4", 61, "F5", 62, "F6",
        63, "F7", 64, "F8", 65, "F9", 66, "F10", 67, "F11", 68, "F12", 69, "PRINTSCREEN", 70, "SCROLLLOCK", 71, "PAUSE", 72, "INSERT", 73, "HOME", 74,
        "PAGEUP", 75, "DELETE", 76, "END", 77, "PAGEDOWN", 78, "RIGHT", 79, "LEFT", 80, "DOWN", 81, "UP", 82, "NUMLOCKCLEAR", 83, "KP_DIVIDE", 84,
        "KP_MULTIPLY", 85, "KP_MINUS", 86, "KP_PLUS", 87, "KP_ENTER", 88, "KP_1", 89, "KP_2", 90, "KP_3", 91, "KP_4", 92, "KP_5", 93, "KP_6", 94,
        "KP_7", 95, "KP_8", 96, "KP_9", 97, "KP_0", 98, "KP_PERIOD", 99, "NONUSBACKSLASH", 100, "APPLICATION", 101, "POWER", 102, "KP_EQUALS", 103,
        "F13", 104, "F14", 105, "F15", 106, "F16", 107, "F17", 108, "F18", 109, "F19", 110, "F20", 111, "F21", 112, "F22", 113, "F23", 114, "F24",
        115, "EXECUTE", 116, "HELP", 117, "MENU", 118, "SELECT", 119, "STOP", 120, "AGAIN", 121, "UNDO", 122, "CUT", 123, "COPY", 124, "PASTE", 125,
        "FIND", 126, "MUTE", 127, "VOLUMEUP", 128, "VOLUMEDOWN", 129, "KP_COMMA", 133, "KP_EQUALSAS400", 134, "INTERNATIONAL1", 135, "INTERNATIONAL2",
        136, "INTERNATIONAL3", 137, "INTERNATIONAL4", 138, "INTERNATIONAL5", 139, "INTERNATIONAL6", 140, "INTERNATIONAL7", 141, "INTERNATIONAL8", 142,
        "INTERNATIONAL9", 143, "LANG1", 144, "LANG2", 145, "LANG3", 146, "LANG4", 147, "LANG5", 148, "LANG6", 149, "LANG7", 150, "LANG8", 151,
        "LANG9", 152, "ALTERASE", 153, "SYSREQ", 154, "CANCEL", 155, "CLEAR", 156, "PRIOR", 157, "RETURN2", 158, "SEPARATOR", 159, "OUT", 160, "OPER",
        161, "CLEARAGAIN", 162, "CRSEL", 163, "EXSEL", 164, "KP_00", 176, "KP_000", 177, "THOUSANDSSEPARATOR", 178, "DECIMALSEPARATOR", 179,
        "CURRENCYUNIT", 180, "CURRENCYSUBUNIT", 181, "KP_LEFTPAREN", 182, "KP_RIGHTPAREN", 183, "KP_LEFTBRACE", 184, "KP_RIGHTBRACE", 185, "KP_TAB",
        186, "KP_BACKSPACE", 187, "KP_A", 188, "KP_B", 189, "KP_C", 190, "KP_D", 191, "KP_E", 192, "KP_F", 193, "KP_XOR", 194, "KP_POWER", 195,
        "KP_PERCENT", 196, "KP_LESS", 197, "KP_GREATER", 198, "KP_AMPERSAND", 199, "KP_DBLAMPERSAND", 200, "KP_VERTICALBAR", 201, "KP_DBLVERTICALBAR",
        202, "KP_COLON", 203, "KP_HASH", 204, "KP_SPACE", 205, "KP_AT", 206, "KP_EXCLAM", 207, "KP_MEMSTORE", 208, "KP_MEMRECALL", 209, "KP_MEMCLEAR",
        210, "KP_MEMADD", 211, "KP_MEMSUBTRACT", 212, "KP_MEMMULTIPLY", 213, "KP_MEMDIVIDE", 214, "KP_PLUSMINUS", 215, "KP_CLEAR", 216,
        "KP_CLEARENTRY", 217, "KP_BINARY", 218, "KP_OCTAL", 219, "KP_DECIMAL", 220, "KP_HEXADECIMAL", 221, "LCTRL", 224, "LSHIFT", 225, "LALT", 226,
        "LGUI", 227, "RCTRL", 228, "RSHIFT", 229, "RALT", 230, "RGUI", 231, "MODE", 257, "AUDIONEXT", 258, "AUDIOPREV", 259, "AUDIOSTOP", 260,
        "AUDIOPLAY", 261, "AUDIOMUTE", 262, "MEDIASELECT", 263, "WWW", 264, "MAIL", 265, "CALCULATOR", 266, "COMPUTER", 267, "AC_SEARCH", 268,
        "AC_HOME", 269, "AC_BACK", 270, "AC_FORWARD", 271, "AC_STOP", 272, "AC_REFRESH", 273, "AC_BOOKMARKS", 274, "BRIGHTNESSDOWN", 275,
        "BRIGHTNESSUP", 276, "DISPLAYSWITCH", 277, "KBDILLUMTOGGLE", 278, "KBDILLUMDOWN", 279, "KBDILLUMUP", 280, "EJECT", 281, "SLEEP", 282, "APP1",
        283, "APP2", 284, "AUDIOREWIND", 285, "AUDIOFASTFORWARD", 286, "NUM_SCANCODES", 512);

    // User Types -------------------------------------------------------------------------------------------------------------------------

    // Fonts (Not node based)
    module.new_usertype<IFont>("IFont", sol::no_constructor);
    module.new_usertype<TtfFont>("TtfFont", sol::no_constructor, sol::base_classes, sol::bases<IFont>());
    module.new_usertype<SpriteFont>("SpriteFont", sol::no_constructor, sol::base_classes, sol::bases<IFont>());

    // Image (Not node based)
    module.new_usertype<Image>("Image", sol::no_constructor);

    // Node
    auto nodeType = module.new_usertype<Node>("Node", sol::no_constructor);
    nodeType["removeAllChildren"] = &Node::RemoveAllChildren;
    nodeType["appendChild"] = &Node::AppendChild;
    nodeType["removeChild"] = &Node::RemoveChild;
    nodeType["getPosition"] = &Node::GetPosition;
    nodeType["setPosition"] = &Node::SetPosition;
    nodeType["onUpdate"] = &Node::SetLuaOnUpdateHandler;
    nodeType["visible"] = sol::property(&Node::GetVisible, &Node::SetVisible);
    nodeType["active"] = sol::property(&Node::GetActive, &Node::SetActive);
    nodeType["data"] = sol::property(&Node::GetLuaTable, &Node::SetLuaTable);

    // Button
    auto buttonType = CreateLuaObjectType<Button>(module, "Button", sol::no_constructor);
    buttonType["setSegments"] = &Button::SetSegments;
    buttonType["setFixedSize"] = &Button::SetSize;
    buttonType["checked"] = sol::property(&Button::GetChecked, &Button::SetChecked);
    buttonType["setPressedOffset"] = &Button::SetPressedOffset;
    buttonType["setFrameIndex"] = &Button::LuaSetFrameIndex;
    buttonType["onActivate"] = &Button::LuaSetActivateCallback;
    buttonType["onPressed"] = &Button::LuaSetPressCallback;
    buttonType["disabled"] = sol::property(&Button::GetDisabled, &Button::SetDisabled);
    buttonType["onMouseEnter"] = &Button::LuaSetMouseEnterCallback;
    buttonType["onMouseLeave"] = &Button::LuaSetMouseLeaveCallback;

    // Label
    auto labelType = CreateLuaObjectType<Label>(module, "Label", sol::no_constructor);
    labelType["caption"] = sol::property(&Label::GetCaption, &Label::SetCaption);
    labelType["setAlignment"] = &Label::SetAlignmentStr;
    labelType["setColorMod"] = &Label::SetColorMod;
    labelType["blendMode"] = sol::property(&Label::LuaGetBlendMode, &Label::LuaSetBlendMode);
    labelType["bold"] = sol::property(&Label::GetBold, &Label::SetBold);
    labelType["italic"] = sol::property(&Label::GetItalic, &Label::SetItalic);
    labelType["underline"] = sol::property(&Label::GetUnderline, &Label::SetUnderline);
    labelType["strikethrough"] = sol::property(&Label::GetStrikethrough, &Label::SetStrikethrough);
    labelType["verticalSpacing"] = sol::property(&Label::GetVerticalSpacing, &Label::SetVerticalSpacing);

    // Sprite
    auto spriteType = CreateLuaObjectType<Sprite>(module, "Sprite", sol::no_constructor);
    spriteType["setCellSize"] = &Sprite::SetCellSize;
    spriteType["getCellSize"] = &Sprite::GetCellSize;
    spriteType["onMouseButtonDown"] = &Sprite::SetLuaMouseButtonDownHandler;
    spriteType["onMouseButtonUp"] = &Sprite::SetLuaMouseButtonUpHandler;
    spriteType["blendMode"] = sol::property(&Sprite::LuaGetBlendMode, &Sprite::LuaSetBlendMode);
    spriteType["bottomOrigin"] = sol::property(&Sprite::GetIsBottomOrigin, &Sprite::SetIsBottomOrigin);
    spriteType["playMode"] = sol::property(&Sprite::LuaGetPlayMode, &Sprite::LuaSetPlayMode);
    spriteType["currentFrameIndex"] = sol::property(&Sprite::GetCurrentFrameIndex, &Sprite::SetCurrentFrameIndex);

    // InputListener
    auto inputListenerType = CreateLuaObjectType<InputListener>(module, "InputListener", sol::no_constructor);
    inputListenerType["onMouseButton"] = &InputListener::LuaSetMouseButtonCallback;
    inputListenerType["onMouseMove"] = &InputListener::LuaSetMouseMoveCallback;
    inputListenerType["onKey"] = &InputListener::LuaSetKeyCallback;

    // Sound Effect
    auto soundEffect = module.new_usertype<SoundEffect>("SoundEffect", sol::no_constructor);
    soundEffect["play"] = &SoundEffect::Play;
    soundEffect["stop"] = &SoundEffect::Stop;
    soundEffect["pause"] = &SoundEffect::Pause;
    soundEffect["isPlaying"] = &SoundEffect::GetIsPlaying;
    soundEffect["isPaused"] = &SoundEffect::GetIsPaused;
    soundEffect["volume"] = sol::property(&SoundEffect::GetVolume, &SoundEffect::SetVolume);
    soundEffect["loop"] = sol::property(&SoundEffect::GetLoop, &SoundEffect::SetLoop);

    // Map Renderer
    auto mapRenderer = CreateLuaObjectType<MapRenderer>(module, "MapRenderer", sol::no_constructor);
    mapRenderer["showOuterBorder"] = sol::property(&MapRenderer::ShowOuterBorder, &MapRenderer::ShowOuterBorder);
    mapRenderer["compile"] = &MapRenderer::Compile;

    // Level Type
    auto levelType = module.new_usertype<LibAbyss::LevelType>("LevelType");
    levelType["files"] = sol::property(&LibAbyss::LevelType::Files, &LibAbyss::LevelType::Files);
    levelType["name"] = sol::property(&LibAbyss::LevelType::Name, &LibAbyss::LevelType::Name);
    levelType["id"] = sol::property(&LibAbyss::LevelType::ID, &LibAbyss::LevelType::ID);
    levelType["act"] = sol::property(&LibAbyss::LevelType::Act, &LibAbyss::LevelType::Act);
    levelType["beta"] = sol::property(&LibAbyss::LevelType::Beta, &LibAbyss::LevelType::Beta);
    levelType["expansion"] = sol::property(&LibAbyss::LevelType::Expansion, &LibAbyss::LevelType::Expansion);

    // Level Preset
    auto levelPreset = module.new_usertype<LibAbyss::LevelPreset>("LevelPreset");
    levelPreset["files"] = sol::property(&LibAbyss::LevelPreset::Files, &LibAbyss::LevelPreset::Files);
    levelPreset["name"] = sol::property(&LibAbyss::LevelPreset::Name, &LibAbyss::LevelPreset::Name);
    levelPreset["definitionId"] = sol::property(&LibAbyss::LevelPreset::DefinitionID, &LibAbyss::LevelPreset::DefinitionID);
    levelPreset["levelId"] = sol::property(&LibAbyss::LevelPreset::LevelID, &LibAbyss::LevelPreset::LevelID);
    levelPreset["sizeX"] = sol::property(&LibAbyss::LevelPreset::SizeX, &LibAbyss::LevelPreset::SizeX);
    levelPreset["sizeY"] = sol::property(&LibAbyss::LevelPreset::SizeY, &LibAbyss::LevelPreset::SizeY);
    levelPreset["pops"] = sol::property(&LibAbyss::LevelPreset::Pops, &LibAbyss::LevelPreset::Pops);
    levelPreset["popPad"] = sol::property(&LibAbyss::LevelPreset::PopPad, &LibAbyss::LevelPreset::PopPad);
    levelPreset["dt1Mask"] = sol::property(&LibAbyss::LevelPreset::DT1Mask, &LibAbyss::LevelPreset::DT1Mask);
    levelPreset["populate"] = sol::property(&LibAbyss::LevelPreset::Populate, &LibAbyss::LevelPreset::Populate);
    levelPreset["logicals"] = sol::property(&LibAbyss::LevelPreset::Logicals, &LibAbyss::LevelPreset::Logicals);
    levelPreset["outdoors"] = sol::property(&LibAbyss::LevelPreset::Outdoors, &LibAbyss::LevelPreset::Outdoors);
    levelPreset["animate"] = sol::property(&LibAbyss::LevelPreset::Animate, &LibAbyss::LevelPreset::Animate);
    levelPreset["killEdge"] = sol::property(&LibAbyss::LevelPreset::KillEdge, &LibAbyss::LevelPreset::KillEdge);
    levelPreset["fillBlanks"] = sol::property(&LibAbyss::LevelPreset::FillBlanks, &LibAbyss::LevelPreset::FillBlanks);
    levelPreset["autoMap"] = sol::property(&LibAbyss::LevelPreset::AutoMap, &LibAbyss::LevelPreset::AutoMap);
    levelPreset["scan"] = sol::property(&LibAbyss::LevelPreset::Scan, &LibAbyss::LevelPreset::Scan);
    levelPreset["beta"] = sol::property(&LibAbyss::LevelPreset::Beta, &LibAbyss::LevelPreset::Beta);
    levelPreset["expansion"] = sol::property(&LibAbyss::LevelPreset::Expansion, &LibAbyss::LevelPreset::Expansion);

    // DS1
    auto ds1 = module.new_usertype<LibAbyss::DS1>("DS1", sol::no_constructor);
    ds1["width"] = sol::property(&LibAbyss::DS1::Width, &LibAbyss::DS1::Width);
    ds1["height"] = sol::property(&LibAbyss::DS1::Height, &LibAbyss::DS1::Height);

    // DT1 Tile
    auto dt1TileType = module.new_usertype<LibAbyss::DT1::Tile>("DT1Tile", sol::no_constructor);
    dt1TileType["type"] = sol::readonly_property(&LibAbyss::DT1::Tile::Type);
    dt1TileType["mainIndex"] = sol::readonly_property(&LibAbyss::DT1::Tile::MainIndex);
    dt1TileType["subIndex"] = sol::readonly_property(&LibAbyss::DT1::Tile::SubIndex);

    // Zone
    auto zoneType = module.new_usertype<LibAbyss::Zone>("Zone", sol::no_constructor);
    zoneType["resetMap"] = &LibAbyss::Zone::ResetMap;
    zoneType["stamp"] = &LibAbyss::Zone::Stamp;
    zoneType["getTileInfo"] = &LibAbyss::Zone::GetTileInfo;
    zoneType["width"] = sol::readonly_property(&LibAbyss::Zone::WidthInTiles);
    zoneType["height"] = sol::readonly_property(&LibAbyss::Zone::HeightInTiles);

    _environment.add(module);
}

std::tuple<sol::object, sol::object> AbyssEngine::ScriptHost::LuaLoadString(const std::string_view str, std::string_view chunkName) {
    if (!str.empty() && str[0] == LUA_SIGNATURE[0])
        return std::make_tuple(sol::nil, sol::make_object(_lua, "Bytecode prohibited by Lua sandbox"));

    sol::load_result result = _lua.load(str, std::string(chunkName), sol::load_mode::text);

    if (!result.valid()) {
        sol::error err = result;
        return std::make_tuple(sol::nil, sol::make_object(_lua, err.what()));
    }

    sol::function func = result;
    _environment.set_on(func);

    return std::make_tuple(func, sol::nil);
}

void AbyssEngine::ScriptHost::ExecuteFile(std::string_view path) { LuaLoadFile(path); }

std::tuple<sol::object, sol::object> AbyssEngine::ScriptHost::LuaLoadFile(std::string_view pathStr) {
    std::filesystem::path path(pathStr);

    if (!path.has_extension())
        path += ".lua";

    // TODO: Ensure we haven't already loaded this

    if (!_engine->GetLoader().FileExists(path)) {
        return std::make_tuple(sol::nil, sol::make_object(_lua, "Path does not exist."));
    }

    auto stream = _engine->GetLoader().Load(path);

    std::istreambuf_iterator<char> eos;
    std::string str(std::istreambuf_iterator<char>(stream), eos);

    auto ret = LuaLoadString(str, "@" + path.string());

    SPDLOG_TRACE("Loaded file {0}", path.string());

    try {
        if (std::get<0>(ret) == sol::nil) {
            throw sol::error(std::get<1>(ret).as<std::string>());
        }
    } catch (std::runtime_error &e) {
        auto err = fmt::format("Error loading file {0}: {1}", path.string(), e.what());
        Engine::Get()->Panic(err);
        return std::tuple<sol::nil_t, sol::nil_t>();
    }

    sol::unsafe_function func = std::get<0>(ret);
    return func();
}

void AbyssEngine::ScriptHost::ExecuteString(std::string_view code) {
    auto result = _lua.script(code, _environment);
    if (!result.valid()) {
        const sol::error e = result;
        auto err = fmt::format("Script Error: {0}", e.what());
        Engine::Get()->Panic(err);
        return;
    }
}

sol::object AbyssEngine::ScriptHost::LuaDoFile(std::string_view path) {
    std::tuple<sol::object, sol::object> ret = LuaLoadFile(path);
    if (std::get<0>(ret) == sol::nil) {
        auto e = sol::error(std::get<1>(ret).as<std::string>());
        auto err = fmt::format("Script Error: {0}", e.what());
        Engine::Get()->Panic(err);
        return sol::nil;
    }

    SPDLOG_TRACE("Loaded file {0}", path);
    sol::unsafe_function func = std::get<0>(ret);
    return func();
}

void AbyssEngine::ScriptHost::LuaFuncShutdown() {
    SPDLOG_INFO("Shutting down engine");
    _engine->Stop();
}

std::string_view AbyssEngine::ScriptHost::LuaGetConfig(std::string_view category, std::string_view value) {
    return _engine->GetIniFile().GetValue(category, value);
}

void AbyssEngine::ScriptHost::LuaShowSystemCursor(bool show) { _engine->ShowSystemCursor(show); }

void AbyssEngine::ScriptHost::LuaLog(std::string_view level, std::string_view message) {
    if (level == "info") {
        SPDLOG_INFO(message);
        return;
    }

    if (level == "error") {
        SPDLOG_ERROR(message);
        return;
    }

    if (level == "fatal") {
        SPDLOG_CRITICAL(message);
        return;
    }

    if (level == "warn") {
        SPDLOG_WARN(message);
        return;
    }

    if (level == "debug") {
        SPDLOG_DEBUG(message);
        return;
    }

    if (level == "trace") {
        SPDLOG_TRACE(message);
        return;
    }

    throw std::runtime_error("Unknown log level specified: " + std::string(level));
}

void AbyssEngine::ScriptHost::LuaAddLoaderProvider(std::string_view providerType, std::string_view providerPath) {
    std::filesystem::path path(providerPath);
    std::unique_ptr<Provider> provider;

    if (providerType == "mpq") {
        provider = std::make_unique<AbyssEngine::MPQProvider>(path);
    } else if (providerType == "casc") {
        provider = std::make_unique<AbyssEngine::CASCProvider>(path);
    } else if (providerType == "filesystem") {
        provider = std::make_unique<AbyssEngine::FileSystemProvider>(path);
    } else {
        throw std::runtime_error(absl::StrCat("Unknown provider type: ", providerType));
    }

    _engine->GetLoader().AddProvider(std::move(provider));
}

void AbyssEngine::ScriptHost::LuaCreatePalette(std::string_view paletteName, std::string_view path) {
    bool isDat = !absl::AsciiStrToLower(path).ends_with(".pl2");
    std::filesystem::path filePath(path);
    auto stream = _engine->GetLoader().Load(filePath);
    LibAbyss::Palette palette(stream, isDat);
    _engine->AddPalette(paletteName, palette);
}

bool AbyssEngine::ScriptHost::LuaFileExists(std::string_view fileName) {
    auto path = std::filesystem::path(fileName);
    return _engine->GetLoader().FileExists(path);
}

std::unique_ptr<AbyssEngine::Image> AbyssEngine::ScriptHost::LuaLoadImage(std::string_view spritePath, std::string_view paletteName) {
    const auto &engine = _engine;
    const std::filesystem::path path(spritePath);

    if (!engine->GetLoader().FileExists(path))
        throw std::runtime_error("File not found: " + std::string(spritePath));

    auto stream = engine->GetLoader().Load(path);

    std::string lower = absl::AsciiStrToLower(spritePath);
    if (lower.ends_with(".dc6")) {
        const auto &palette = engine->GetPalette(paletteName);
        return std::make_unique<DC6Sprite>(stream, palette);
    } else if (lower.ends_with(".sprite")) {
        return std::make_unique<D2RSprite>(stream);
    } else
        throw std::runtime_error(absl::StrCat("Unknowns sprite format for file: ", spritePath));
}

std::unique_ptr<AbyssEngine::Sprite> AbyssEngine::ScriptHost::LuaCreateSprite(AbyssEngine::Image &image) { return std::make_unique<Sprite>(image); }

std::unique_ptr<AbyssEngine::Button> AbyssEngine::ScriptHost::LuaCreateButton(Image &image) { return std::make_unique<Button>(image); }

void AbyssEngine::ScriptHost::LuaSetCursor(Sprite &sprite, int offsetX, int offsetY) { _engine->SetCursorSprite(&sprite, offsetX, offsetY); }

AbyssEngine::Node &AbyssEngine::ScriptHost::LuaGetRootNode() { return _engine->GetRootNode(); }

void AbyssEngine::ScriptHost::LuaPlayVideo(std::string_view videoPath, const sol::safe_function &callback) {
    auto stream = _engine->GetLoader().Load(std::filesystem::path(videoPath));
    _engine->PlayVideo(videoPath, std::move(stream), std::nullopt, callback);
}

void AbyssEngine::ScriptHost::LuaPlayVideoAndAudio(std::string_view videoPath, std::string_view audioPath, const sol::safe_function &callback) {
    auto stream = _engine->GetLoader().Load(std::filesystem::path(videoPath));
    auto audio = _engine->GetLoader().Load(std::filesystem::path(audioPath));
    _engine->PlayVideo(videoPath, std::move(stream), std::move(audio), callback);
}

template <class T, typename X>
sol::basic_usertype<T, sol::basic_reference<false>> AbyssEngine::ScriptHost::CreateLuaObjectType(sol::table &module, std::string_view name,
                                                                                                 X &&constructor) {
    auto val = module.new_usertype<T>(name, "new", std::forward<X>(constructor), sol::base_classes, sol::bases<Node>());
    return val;
}

void AbyssEngine::ScriptHost::LuaResetMouseState() { _engine->ResetMouseButtonState(); }

std::string AbyssEngine::ScriptHost::LuaLoadText(std::string_view filePath) {
    if (!_engine->GetLoader().FileExists(filePath))
        throw std::runtime_error(absl::StrCat("Path does not exist: ", filePath));

    auto stream = _engine->GetLoader().Load(filePath);
    std::string result;
    result.resize(stream.size());
    stream.read(result.data(), stream.size());
    return result;
}

std::unique_ptr<AbyssEngine::SpriteFont> AbyssEngine::ScriptHost::LuaCreateSpriteFont(std::string_view fontPath, std::string_view paletteName) {
    return std::make_unique<SpriteFont>(fontPath, paletteName);
}

std::unique_ptr<AbyssEngine::TtfFont> AbyssEngine::ScriptHost::LuaCreateTtfFont(std::string_view fontPath, int size, std::string_view hinting) {
    ITtf::Hinting hint;
    if (hinting == "light") {
        hint = ITtf::Hinting::Light;
    } else if (hinting == "mono") {
        hint = ITtf::Hinting::Mono;
    } else if (hinting == "normal") {
        hint = ITtf::Hinting::Normal;
    } else if (hinting == "none") {
        hint = ITtf::Hinting::None;
    } else {
        throw std::runtime_error("Unknown hinting type");
    }
    return std::make_unique<TtfFont>(fontPath, size, hint);
}

std::unique_ptr<AbyssEngine::Label> AbyssEngine::ScriptHost::LuaCreateLabel(AbyssEngine::IFont &font) {
    if (auto *spriteFont = dynamic_cast<SpriteFont *>(&font)) {
        return std::make_unique<SpriteLabel>(*spriteFont);
    }
    if (auto *ttfFont = dynamic_cast<TtfFont *>(&font)) {
        return std::make_unique<TtfLabel>(*ttfFont);
    }
    throw std::runtime_error("Unknown font type for the label");
}

void AbyssEngine::ScriptHost::GC() { _lua.collect_garbage(); }

void AbyssEngine::ScriptHost::LuaPlayBackgroundMusic(std::string_view fileName) {
    if (fileName.empty()) {
        _engine->GetSystemIO().SetBackgroundMusic(nullptr);
        return;
    }

    auto &loader = _engine->GetLoader();

    if (!loader.FileExists(fileName))
        throw std::runtime_error(absl::StrCat("File not found: ", fileName));

    auto stream = loader.Load(fileName);
    auto result = std::make_unique<LibAbyss::AudioStream>(std::move(stream));
    result->SetLoop(true);
    result->Play();
    _engine->GetSystemIO().SetBackgroundMusic(std::move(result));
}

std::unique_ptr<AbyssEngine::SoundEffect> AbyssEngine::ScriptHost::LuaCreateSoundEffect(std::string_view fileName) {
    auto &loader = _engine->GetLoader();

    if (!loader.FileExists(fileName))
        throw std::runtime_error(absl::StrCat("File not found: ", fileName));

    auto stream = loader.Load(fileName);
    auto audioStream = std::make_unique<LibAbyss::AudioStream>(std::move(stream));

    return std::make_unique<SoundEffect>(std::move(audioStream));
}

std::unique_ptr<AbyssEngine::MapRenderer> AbyssEngine::ScriptHost::LuaCreateMapRenderer(LibAbyss::Zone *zone) {
    return std::make_unique<MapRenderer>(zone);
}

std::unique_ptr<LibAbyss::Zone> AbyssEngine::ScriptHost::LuaCreateZone() {
    return std::make_unique<LibAbyss::Zone>([this](std::string_view fileName) -> LibAbyss::DT1 {
        auto stream = _engine->GetLoader().Load(fileName);
        return LibAbyss::DT1(stream);
    });
}
std::unique_ptr<LibAbyss::DS1> AbyssEngine::ScriptHost::LuaLoadDS1(std::string_view fileName) {
    if (!_engine->GetLoader().FileExists(fileName))
        throw std::runtime_error(absl::StrCat("File not found: ", fileName));

    auto stream = _engine->GetLoader().Load(fileName);
    return std::make_unique<LibAbyss::DS1>(stream);
}

std::unique_ptr<AbyssEngine::InputListener> AbyssEngine::ScriptHost::LuaCreateInputListener() { return std::make_unique<InputListener>(); }

std::tuple<int, int> AbyssEngine::ScriptHost::LuaWorldToOrtho(int x, int y) {
    MapRenderer::WorldToOrtho(x, y);
    return {x, y};
}

std::tuple<int, int> AbyssEngine::ScriptHost::LuaOrthoToWorld(int x, int y) {
    MapRenderer::OrthoToWorld(x, y);
    return {x, y};
}
std::string AbyssEngine::ScriptHost::LuaUtf16To8(const std::string &str) {
    std::string result;
    result.reserve(str.length());

    int i = 0;
    bool skip = false;

    for (auto ch : str) {
        if (i++ < 2)
            continue;

        if (skip) {
            skip = false;
            continue;
        }
        skip = true;
        result += (char)ch;
    }

    return result;
}
bool AbyssEngine::ScriptHost::LuaIsKeyPressed(uint16_t scancode) { return _engine->GetSystemIO().IsKeyPressed(scancode); }
