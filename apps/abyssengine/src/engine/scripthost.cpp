#include "scripthost.h"
#include "../node/d2rsprite.h"
#include "../node/dc6sprite.h"
#include "cascprovider.h"
#include "engine.h"
#include "filesystemprovider.h"
#include "font.h"
#include "image.h"
#include "mpqprovider.h"
#include <absl/container/flat_hash_map.h>
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <config.h>
#include <filesystem>
#include <libabyss/common/levelpreset.h>
#include <libabyss/common/leveltype.h>
#include <libabyss/formats/d2/dt1.h>
#include <libabyss/formats/d2/tbl.h>
#include <memory>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

extern "C" {
int luaopen_lpeg(lua_State *L);
int luaopen_lsqlite3(lua_State *L);
}

namespace AbyssEngine {
namespace {
template <typename T, typename N> void AddNodeFunctionProperties(N &nodeType) {
    // Functions work via sol::base_classes, but are slow; properties don't work at all.
    // So create both explicitly
    nodeType["nodeType"] = &T::NodeType;
    nodeType["removeAllChildren"] = &T::RemoveAllChildren;
    nodeType["appendChild"] = &T::AppendChild;
    nodeType["removeChild"] = &T::RemoveChild;
    nodeType["getChildren"] = &T::GetChildren;
    nodeType["getPosition"] = &T::GetPosition;
    nodeType["setPosition"] = &T::SetPosition;
    nodeType["onUpdate"] = &T::SetLuaOnUpdateHandler;
    nodeType["visible"] = sol::property(&T::GetVisible, &T::SetVisible);
    nodeType["active"] = sol::property(&T::GetActive, &T::SetActive);
    nodeType["data"] = sol::property(&T::GetLuaTable, &T::SetLuaTable);
}

} // namespace

template <typename T> std::unique_ptr<T> ScriptHost::InitializeTableFor(std::unique_ptr<T> node) {
    node->SetLuaTable(_lua.create_table());
    return node;
}

ScriptHost::ScriptHost(Engine *engine) : _engine(engine), _lua() {
    _lua.stop_gc();
    _lua.open_libraries();
    _lua.require("lpeg", luaopen_lpeg);
    _lua.require("lsqlite3", luaopen_lsqlite3);

    _environment = sol::environment(_lua, sol::create, _lua.globals());
    sol::table module = _lua.create_table("abyss");

    // Overload loading functions ---------------------------------------------------------------------------------------------------------

    _environment.set_function("loadstring", &ScriptHost::LuaLoadString, this);
    _environment.set_function("loadfile", &ScriptHost::LuaLoadFile, this);
    _environment.set_function("dofile", &ScriptHost::LuaDoFile, this);
    _environment.set_function("require", &ScriptHost::LuaLoadFile, this);

    // Engine Functions -------------------------------------------------------------------------------------------------------------------

    module.set_function("createButton", [this](Image &image) { return InitializeTableFor(LuaCreateButton(image)); });
    module.set_function("createInputListener", [this]() { return InitializeTableFor(LuaCreateInputListener()); });
    module.set_function("createLabel", [this](IFont &font) { return InitializeTableFor(LuaCreateLabel(font)); });
    module.set_function("createMapRenderer", [this](LibAbyss::Zone *zone) { return InitializeTableFor(LuaCreateMapRenderer(zone)); });
    module.set_function("createNode", [this]() { return InitializeTableFor(LuaCreateNode()); });
    module.set_function("createSprite", [this](Image &image) { return InitializeTableFor(LuaCreateSprite(image)); });

    module.set_function("addLoaderProvider", &ScriptHost::LuaAddLoaderProvider, this);
    module.set_function("createPalette", &ScriptHost::LuaCreatePalette, this);
    module.set_function("createSoundEffect", &ScriptHost::LuaCreateSoundEffect, this);
    module.set_function("createSpriteFont", &ScriptHost::LuaCreateSpriteFont, this);
    module.set_function("createTtfFont", &ScriptHost::LuaCreateTtfFont, this);
    module.set_function("getBuildType", &ScriptHost::LuaGetBuildType, this);
    module.set_function("getCpuType", &ScriptHost::LuaGetCpuType, this);
    module.set_function("getPlatform", &ScriptHost::LuaGetPlatform, this);
    module.set_function("getScreenSize", &ScriptHost::LuaGetScreenSize, this);
    module.set_function("getEngineVersion", &ScriptHost::LuaGetEngineVersion, this);
    module.set_function("isKeyPressed", &ScriptHost::LuaIsKeyPressed, this);
    module.set_function("loadDS1", &ScriptHost::LuaLoadDS1, this);
    module.set_function("loadImage", &ScriptHost::LuaLoadImage, this);
    module.set_function("loadString", &ScriptHost::LuaLoadText, this);
    module.set_function("loadTbl", &ScriptHost::LuaLoadTbl, this);
    module.set_function("createZone", &ScriptHost::LuaCreateZone, this);
    module.set_function("fileExists", &ScriptHost::LuaFileExists, this);
    module.set_function("getConfig", &ScriptHost::LuaGetConfig, this);
    module.set_function("getRootNode", &ScriptHost::LuaGetRootNode, this);
    module.set_function("log", &ScriptHost::LuaLog, this);
    module.set_function("orthoToWorld", &ScriptHost::LuaOrthoToWorld, this);
    module.set_function("panic", &ScriptHost::LuaPanic, this);
    module.set_function("playBackgroundMusic", &ScriptHost::LuaPlayBackgroundMusic, this);
    module.set_function("playVideo", &ScriptHost::LuaPlayVideo, this);
    module.set_function("playVideoAndAudio", &ScriptHost::LuaPlayVideoAndAudio, this);
    module.set_function("resetMouseState", &ScriptHost::LuaResetMouseState, this);
    module.set_function("setCursor", &ScriptHost::LuaSetCursor, this);
    module.set_function("setWindowTitle", &ScriptHost::LuaSetWindowTitle, this);
    module.set_function("showSystemCursor", &ScriptHost::LuaShowSystemCursor, this);
    module.set_function("shutdown", &ScriptHost::LuaFuncShutdown, this);
    module.set_function("utf16To8", &ScriptHost::LuaUtf16To8, this);
    module.set_function("worldToOrtho", &ScriptHost::LuaWorldToOrtho, this);

    DefineScanCodes(module);

    // User Types -------------------------------------------------------------------------------------------------------------------------

    // Fonts (Not node based)
    module.new_usertype<IFont>("IFont", sol::no_constructor);
    module.new_usertype<TtfFont>("TtfFont", sol::no_constructor, sol::base_classes, sol::bases<IFont>());
    module.new_usertype<SpriteFont>("SpriteFont", sol::no_constructor, sol::base_classes, sol::bases<IFont>());

    // Image (Not node based)
    auto imageType = module.new_usertype<Image>("Image", sol::no_constructor);
    imageType["getFrameSize"] = &Image::LuaGetFrameSize;
    imageType["getNumberOfFrames"] = &Image::GetFramesPerAnimation;

    // Node
    _nodeType = module.new_usertype<Node>("Node", sol::no_constructor);
    AddNodeFunctionProperties<Node>(_nodeType);

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
    labelType["maxWidth"] = sol::property(&Label::GetMaxWidth, &Label::SetMaxWidth);

    // Sprite
    auto spriteType = CreateLuaObjectType<Sprite>(module, "Sprite", sol::no_constructor);
    spriteType["blendMode"] = sol::property(&Sprite::LuaGetBlendMode, &Sprite::LuaSetBlendMode);
    spriteType["bottomOrigin"] = sol::property(&Sprite::GetIsBottomOrigin, &Sprite::SetIsBottomOrigin);
    spriteType["playMode"] = sol::property(&Sprite::LuaGetPlayMode, &Sprite::LuaSetPlayMode);
    spriteType["currentFrameIndex"] = sol::property(&Sprite::GetCurrentFrameIndex, &Sprite::SetCurrentFrameIndex);
    spriteType["loopAnimation"] = sol::property(&Sprite::GetLoopAnimation, &Sprite::SetLoopAnimation);
    spriteType["setCellSize"] = &Sprite::SetCellSize;
    spriteType["getCellSize"] = &Sprite::GetCellSize;
    spriteType["onMouseButtonDown"] = &Sprite::SetLuaMouseButtonDownHandler;
    spriteType["onMouseButtonUp"] = &Sprite::SetLuaMouseButtonUpHandler;
    spriteType["onAnimationFinished"] = &Sprite::SetLuaAnimationFinishedHandler;

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

std::tuple<sol::object, sol::object> ScriptHost::LuaLoadString(const std::string_view str, std::string_view chunkName) {
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

void ScriptHost::ExecuteFile(std::string_view path) { LuaLoadFile(path); }

std::tuple<sol::object, sol::object> ScriptHost::LuaLoadFile(std::string_view pathStr) {
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

std::string ScriptHost::ExecuteString(std::string_view code) {
    auto result = _lua.script(code, _environment);
    if (!result.valid()) {
        const sol::error e = result;
        auto err = fmt::format("Script Error: {0}", e.what());
        Engine::Get()->Panic(err);
        return "";
    }

    auto resultType = result.get_type();

    switch (resultType) {
    case sol::type::string:
        return result.get<std::string>();
    case sol::type::number:
        return std::to_string(result.get<double>());
    case sol::type::boolean:
        return result.get<bool>() ? "true" : "false";
    default:
    case sol::type::nil:
        return "";
    }
}

sol::object ScriptHost::LuaDoFile(std::string_view path) {
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

void ScriptHost::LuaFuncShutdown() {
    SPDLOG_INFO("Shutting down engine");
    _engine->Stop();
}

std::string_view ScriptHost::LuaGetConfig(std::string_view category, std::string_view value) {
    return _engine->GetIniFile().GetValue(category, value);
}

void ScriptHost::LuaShowSystemCursor(bool show) { _engine->ShowSystemCursor(show); }

void ScriptHost::LuaLog(std::string_view level, std::string_view message) {
    if (level == "info") {
        SPDLOG_INFO("{}", message);
        return;
    }

    if (level == "error") {
        SPDLOG_ERROR("{}", message);
        return;
    }

    if (level == "fatal") {
        SPDLOG_CRITICAL("{}", message);
        return;
    }

    if (level == "warn") {
        SPDLOG_WARN("{}", message);
        return;
    }

    if (level == "debug") {
        SPDLOG_DEBUG("{}", message);
        return;
    }

    if (level == "trace") {
        SPDLOG_TRACE("{}", message);
        return;
    }

    throw std::runtime_error("Unknown log level specified: " + std::string(level));
}

void ScriptHost::LuaAddLoaderProvider(std::string_view providerType, std::string_view providerPath) {
    std::filesystem::path path(providerPath);
    std::unique_ptr<Provider> provider;

    if (providerType == "mpq") {
        provider = std::make_unique<MPQProvider>(path);
    } else if (providerType == "casc") {
        provider = std::make_unique<CASCProvider>(path);
    } else if (providerType == "filesystem") {
        provider = std::make_unique<FileSystemProvider>(path);
    } else {
        throw std::runtime_error(absl::StrCat("Unknown provider type: ", providerType));
    }

    _engine->GetLoader().AddProvider(std::move(provider));
}

void ScriptHost::LuaCreatePalette(std::string_view paletteName, std::string_view path) {
    bool isDat = !absl::AsciiStrToLower(path).ends_with(".pl2");
    std::filesystem::path filePath(path);
    auto stream = _engine->GetLoader().Load(filePath);
    LibAbyss::Palette palette(stream, isDat);
    _engine->AddPalette(paletteName, palette);
}

bool ScriptHost::LuaFileExists(std::string_view fileName) {
    auto path = std::filesystem::path(fileName);
    return _engine->GetLoader().FileExists(path);
}

std::unique_ptr<Image> ScriptHost::LuaLoadImage(std::string_view spritePath, std::string_view paletteName) {
    const auto &engine = _engine;
    const std::filesystem::path path(spritePath);

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

std::unique_ptr<Sprite> ScriptHost::LuaCreateSprite(Image &image) { return std::make_unique<Sprite>(image); }

std::unique_ptr<Button> ScriptHost::LuaCreateButton(Image &image) { return std::make_unique<Button>(image); }

void ScriptHost::LuaSetCursor(Sprite &sprite, int offsetX, int offsetY) { _engine->SetCursorSprite(&sprite, offsetX, offsetY); }

void ScriptHost::LuaSetWindowTitle(std::string_view title) { _engine->SetWindowTitle(title); }

Node &ScriptHost::LuaGetRootNode() { return _engine->GetRootNode(); }

void ScriptHost::LuaPlayVideo(std::string_view videoPath, const sol::safe_function &callback) {
    auto stream = _engine->GetLoader().Load(std::filesystem::path(videoPath));
    _engine->PlayVideo(videoPath, std::move(stream), std::nullopt, callback);
}

void ScriptHost::LuaPlayVideoAndAudio(std::string_view videoPath, std::string_view audioPath, const sol::safe_function &callback) {
    auto stream = _engine->GetLoader().Load(std::filesystem::path(videoPath));
    auto audio = _engine->GetLoader().Load(std::filesystem::path(audioPath));
    _engine->PlayVideo(videoPath, std::move(stream), std::move(audio), callback);
}

template <typename T, typename X>
sol::basic_usertype<T, sol::basic_reference<false>> ScriptHost::CreateLuaObjectType(sol::table &module, std::string_view name, X &&constructor) {
    auto val = module.new_usertype<T>(name, "new", std::forward<X>(constructor), sol::base_classes, sol::bases<Node>());
    AddNodeFunctionProperties<T>(val);
    _nodeType[absl::StrCat("castTo", name)] = [](Node &node) -> T * {
        if (auto *cast = dynamic_cast<T *>(&node)) {
            return cast;
        }
        return nullptr;
    };
    return val;
}

void ScriptHost::LuaResetMouseState() { _engine->ResetMouseButtonState(); }

std::string ScriptHost::LuaLoadText(std::string_view filePath) {
    auto stream = _engine->GetLoader().Load(filePath);
    std::string result;
    result.resize(stream.size());
    stream.read(result.data(), stream.size());
    return result;
}

sol::table ScriptHost::LuaLoadTbl(std::string_view filePath) {
    absl::flat_hash_map<std::string, std::string> table = LibAbyss::ReadTbl(_engine->GetLoader().Load(filePath));

    sol::table result = _lua.create_table();
    for (auto &&[key, value] : std::move(table)) {
        result[key] = std::move(value);
    }

    return result;
}

std::unique_ptr<SpriteFont> ScriptHost::LuaCreateSpriteFont(std::string_view fontPath, std::string_view paletteName, bool useGlyphHeight,
                                                            std::string_view blendMode) {
    return std::make_unique<SpriteFont>(fontPath, paletteName, useGlyphHeight, StringToBlendMode(blendMode));
}

std::unique_ptr<TtfFont> ScriptHost::LuaCreateTtfFont(std::string_view fontPath, int size, std::string_view hinting) {
    //Cairo::HintStyle hint;
    cairo_hint_style_t hint;
    if (hinting == "slight") {
        //hint = Cairo::HINT_STYLE_SLIGHT;
        hint = CAIRO_HINT_STYLE_SLIGHT;
    } else if (hinting == "medium") {
        //hint = Cairo::HINT_STYLE_MEDIUM;
        hint = CAIRO_HINT_STYLE_MEDIUM;
    } else if (hinting == "none") {
        //hint = Cairo::HINT_STYLE_NONE;
        hint = CAIRO_HINT_STYLE_NONE;
    } else if (hinting == "full") {
        //hint = Cairo::HINT_STYLE_FULL;
        hint = CAIRO_HINT_STYLE_FULL;
    } else {
        throw std::runtime_error("Unknown hinting type");
    }
    return std::make_unique<TtfFont>(fontPath, size, hint, CAIRO_ANTIALIAS_SUBPIXEL);
}

std::unique_ptr<Label> ScriptHost::LuaCreateLabel(IFont &font) {
    if (auto *spriteFont = dynamic_cast<SpriteFont *>(&font)) {
        return std::make_unique<SpriteLabel>(*spriteFont);
    }
    if (auto *ttfFont = dynamic_cast<TtfFont *>(&font)) {
        return std::make_unique<TtfLabel>(*ttfFont);
    }
    throw std::runtime_error("Unknown font type for the label");
}

void ScriptHost::GC() { _lua.collect_garbage(); }

void ScriptHost::LuaPlayBackgroundMusic(std::string_view fileName) {
    if (fileName.empty()) {
        _engine->GetSystemIO().SetBackgroundMusic(nullptr);
        return;
    }

    auto &loader = _engine->GetLoader();

    auto stream = loader.Load(fileName);
    auto result = std::make_unique<LibAbyss::AudioStream>(std::move(stream));
    result->SetLoop(true);
    result->Play();
    _engine->GetSystemIO().SetBackgroundMusic(std::move(result));
}

std::unique_ptr<SoundEffect> ScriptHost::LuaCreateSoundEffect(std::string_view fileName) {
    auto &loader = _engine->GetLoader();

    auto stream = loader.Load(fileName);
    auto audioStream = std::make_unique<LibAbyss::AudioStream>(std::move(stream));

    return std::make_unique<SoundEffect>(std::move(audioStream));
}

std::unique_ptr<MapRenderer> ScriptHost::LuaCreateMapRenderer(LibAbyss::Zone *zone) { return std::make_unique<MapRenderer>(zone); }

std::unique_ptr<LibAbyss::Zone> ScriptHost::LuaCreateZone() {
    return std::make_unique<LibAbyss::Zone>([this](std::string_view fileName) -> LibAbyss::DT1 {
        auto stream = _engine->GetLoader().Load(fileName);
        return LibAbyss::DT1(stream);
    });
}
std::unique_ptr<LibAbyss::DS1> ScriptHost::LuaLoadDS1(std::string_view fileName) {
    auto stream = _engine->GetLoader().Load(fileName);
    return std::make_unique<LibAbyss::DS1>(stream);
}

std::unique_ptr<InputListener> ScriptHost::LuaCreateInputListener() { return std::make_unique<InputListener>(); }

std::tuple<int, int> ScriptHost::LuaWorldToOrtho(int x, int y) {
    MapRenderer::WorldToOrtho(x, y);
    return {x, y};
}

std::tuple<int, int> ScriptHost::LuaOrthoToWorld(int x, int y) {
    MapRenderer::OrthoToWorld(x, y);
    return {x, y};
}
std::string ScriptHost::LuaUtf16To8(const std::string &str) {
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
bool ScriptHost::LuaIsKeyPressed(uint16_t scancode) { return _engine->GetSystemIO().IsKeyPressed(scancode); }

std::unique_ptr<Node> ScriptHost::LuaCreateNode() { return std::make_unique<Node>(); }

void ScriptHost::DefineScanCodes(sol::table module) {
    module["scanCodes"] = _lua.create_table();
    module["scanCodes"]["UNKNOWN"] = 0;
    module["scanCodes"]["A"] = 4;
    module["scanCodes"]["B"] = 5;
    module["scanCodes"]["C"] = 6;
    module["scanCodes"]["D"] = 7;
    module["scanCodes"]["E"] = 8;
    module["scanCodes"]["F"] = 9;
    module["scanCodes"]["G"] = 10;
    module["scanCodes"]["H"] = 11;
    module["scanCodes"]["I"] = 12;
    module["scanCodes"]["J"] = 13;
    module["scanCodes"]["K"] = 14;
    module["scanCodes"]["L"] = 15;
    module["scanCodes"]["M"] = 16;
    module["scanCodes"]["N"] = 17;
    module["scanCodes"]["O"] = 18;
    module["scanCodes"]["P"] = 19;
    module["scanCodes"]["Q"] = 20;
    module["scanCodes"]["R"] = 21;
    module["scanCodes"]["S"] = 22;
    module["scanCodes"]["T"] = 23;
    module["scanCodes"]["U"] = 24;
    module["scanCodes"]["V"] = 25;
    module["scanCodes"]["W"] = 26;
    module["scanCodes"]["X"] = 27;
    module["scanCodes"]["Y"] = 28;
    module["scanCodes"]["Z"] = 29;
    module["scanCodes"]["NUM_1"] = 30;
    module["scanCodes"]["NUM_2"] = 31;
    module["scanCodes"]["NUM_3"] = 32;
    module["scanCodes"]["NUM_4"] = 33;
    module["scanCodes"]["NUM_5"] = 34;
    module["scanCodes"]["NUM_6"] = 35;
    module["scanCodes"]["NUM_7"] = 36;
    module["scanCodes"]["NUM_8"] = 37;
    module["scanCodes"]["NUM_9"] = 38;
    module["scanCodes"]["NUM_0"] = 39;
    module["scanCodes"]["RETURN"] = 40;
    module["scanCodes"]["ESCAPE"] = 41;
    module["scanCodes"]["BACKSPACE"] = 42;
    module["scanCodes"]["TAB"] = 43;
    module["scanCodes"]["SPACE"] = 44;
    module["scanCodes"]["MINUS"] = 45;
    module["scanCodes"]["EQUALS"] = 46;
    module["scanCodes"]["LEFTBRACKET"] = 47;
    module["scanCodes"]["RIGHTBRACKET"] = 48;
    module["scanCodes"]["BACKSLASH"] = 49;
    module["scanCodes"]["NONUSHASH"] = 50;
    module["scanCodes"]["SEMICOLON"] = 51;
    module["scanCodes"]["APOSTROPHE"] = 52;
    module["scanCodes"]["GRAVE"] = 53;
    module["scanCodes"]["COMMA"] = 54;
    module["scanCodes"]["PERIOD"] = 55;
    module["scanCodes"]["SLASH"] = 56;
    module["scanCodes"]["CAPSLOCK"] = 57;
    module["scanCodes"]["F1"] = 58;
    module["scanCodes"]["F2"] = 59;
    module["scanCodes"]["F3"] = 60;
    module["scanCodes"]["F4"] = 61;
    module["scanCodes"]["F5"] = 62;
    module["scanCodes"]["F6"] = 63;
    module["scanCodes"]["F7"] = 64;
    module["scanCodes"]["F8"] = 65;
    module["scanCodes"]["F9"] = 66;
    module["scanCodes"]["F10"] = 67;
    module["scanCodes"]["F11"] = 68;
    module["scanCodes"]["F12"] = 69;
    module["scanCodes"]["PRINTSCREEN"] = 70;
    module["scanCodes"]["SCROLLLOCK"] = 71;
    module["scanCodes"]["PAUSE"] = 72;
    module["scanCodes"]["INSERT"] = 73;
    module["scanCodes"]["HOME"] = 74;
    module["scanCodes"]["PAGEUP"] = 75;
    module["scanCodes"]["DELETE"] = 76;
    module["scanCodes"]["END"] = 77;
    module["scanCodes"]["PAGEDOWN"] = 78;
    module["scanCodes"]["RIGHT"] = 79;
    module["scanCodes"]["LEFT"] = 80;
    module["scanCodes"]["DOWN"] = 81;
    module["scanCodes"]["UP"] = 82;
    module["scanCodes"]["NUMLOCKCLEAR"] = 83;
    module["scanCodes"]["KP_DIVIDE"] = 84;
    module["scanCodes"]["KP_MULTIPLY"] = 85;
    module["scanCodes"]["KP_MINUS"] = 86;
    module["scanCodes"]["KP_PLUS"] = 87;
    module["scanCodes"]["KP_ENTER"] = 88;
    module["scanCodes"]["KP_1"] = 89;
    module["scanCodes"]["KP_2"] = 90;
    module["scanCodes"]["KP_3"] = 91;
    module["scanCodes"]["KP_4"] = 92;
    module["scanCodes"]["KP_5"] = 93;
    module["scanCodes"]["KP_6"] = 94;
    module["scanCodes"]["KP_7"] = 95;
    module["scanCodes"]["KP_8"] = 96;
    module["scanCodes"]["KP_9"] = 97;
    module["scanCodes"]["KP_0"] = 98;
    module["scanCodes"]["KP_PERIOD"] = 99;
    module["scanCodes"]["NONUSBACKSLASH"] = 100;
    module["scanCodes"]["APPLICATION"] = 101;
    module["scanCodes"]["POWER"] = 102;
    module["scanCodes"]["KP_EQUALS"] = 103;
    module["scanCodes"]["F13"] = 104;
    module["scanCodes"]["F14"] = 105;
    module["scanCodes"]["F15"] = 106;
    module["scanCodes"]["F16"] = 107;
    module["scanCodes"]["F17"] = 108;
    module["scanCodes"]["F18"] = 109;
    module["scanCodes"]["F19"] = 110;
    module["scanCodes"]["F20"] = 111;
    module["scanCodes"]["F21"] = 112;
    module["scanCodes"]["F22"] = 113;
    module["scanCodes"]["F23"] = 114;
    module["scanCodes"]["F24"] = 115;
    module["scanCodes"]["EXECUTE"] = 116;
    module["scanCodes"]["HELP"] = 117;
    module["scanCodes"]["MENU"] = 118;
    module["scanCodes"]["SELECT"] = 119;
    module["scanCodes"]["STOP"] = 120;
    module["scanCodes"]["AGAIN"] = 121;
    module["scanCodes"]["UNDO"] = 122;
    module["scanCodes"]["CUT"] = 123;
    module["scanCodes"]["COPY"] = 124;
    module["scanCodes"]["PASTE"] = 125;
    module["scanCodes"]["FIND"] = 126;
    module["scanCodes"]["MUTE"] = 127;
    module["scanCodes"]["VOLUMEUP"] = 128;
    module["scanCodes"]["VOLUMEDOWN"] = 129;
    module["scanCodes"]["KP_COMMA"] = 133;
    module["scanCodes"]["KP_EQUALSAS400"] = 134;
    module["scanCodes"]["INTERNATIONAL1"] = 135;
    module["scanCodes"]["INTERNATIONAL2"] = 136;
    module["scanCodes"]["INTERNATIONAL3"] = 137;
    module["scanCodes"]["INTERNATIONAL4"] = 138;
    module["scanCodes"]["INTERNATIONAL5"] = 139;
    module["scanCodes"]["INTERNATIONAL6"] = 140;
    module["scanCodes"]["INTERNATIONAL7"] = 141;
    module["scanCodes"]["INTERNATIONAL8"] = 142;
    module["scanCodes"]["INTERNATIONAL9"] = 143;
    module["scanCodes"]["LANG1"] = 144;
    module["scanCodes"]["LANG2"] = 145;
    module["scanCodes"]["LANG3"] = 146;
    module["scanCodes"]["LANG4"] = 147;
    module["scanCodes"]["LANG5"] = 148;
    module["scanCodes"]["LANG6"] = 149;
    module["scanCodes"]["LANG7"] = 150;
    module["scanCodes"]["LANG8"] = 151;
    module["scanCodes"]["LANG9"] = 152;
    module["scanCodes"]["ALTERASE"] = 153;
    module["scanCodes"]["SYSREQ"] = 154;
    module["scanCodes"]["CANCEL"] = 155;
    module["scanCodes"]["CLEAR"] = 156;
    module["scanCodes"]["PRIOR"] = 157;
    module["scanCodes"]["RETURN2"] = 158;
    module["scanCodes"]["SEPARATOR"] = 159;
    module["scanCodes"]["OUT"] = 160;
    module["scanCodes"]["OPER"] = 161;
    module["scanCodes"]["CLEARAGAIN"] = 162;
    module["scanCodes"]["CRSEL"] = 163;
    module["scanCodes"]["EXSEL"] = 164;
    module["scanCodes"]["KP_00"] = 176;
    module["scanCodes"]["KP_000"] = 177;
    module["scanCodes"]["THOUSANDSSEPARATOR"] = 178;
    module["scanCodes"]["DECIMALSEPARATOR"] = 179;
    module["scanCodes"]["CURRENCYUNIT"] = 180;
    module["scanCodes"]["CURRENCYSUBUNIT"] = 181;
    module["scanCodes"]["KP_LEFTPAREN"] = 182;
    module["scanCodes"]["KP_RIGHTPAREN"] = 183;
    module["scanCodes"]["KP_LEFTBRACE"] = 184;
    module["scanCodes"]["KP_RIGHTBRACE"] = 185;
    module["scanCodes"]["KP_TAB"] = 186;
    module["scanCodes"]["KP_BACKSPACE"] = 187;
    module["scanCodes"]["KP_A"] = 188;
    module["scanCodes"]["KP_B"] = 189;
    module["scanCodes"]["KP_C"] = 190;
    module["scanCodes"]["KP_D"] = 191;
    module["scanCodes"]["KP_E"] = 192;
    module["scanCodes"]["KP_F"] = 193;
    module["scanCodes"]["KP_XOR"] = 194;
    module["scanCodes"]["KP_POWER"] = 195;
    module["scanCodes"]["KP_PERCENT"] = 196;
    module["scanCodes"]["KP_LESS"] = 197;
    module["scanCodes"]["KP_GREATER"] = 198;
    module["scanCodes"]["KP_AMPERSAND"] = 199;
    module["scanCodes"]["KP_DBLAMPERSAND"] = 200;
    module["scanCodes"]["KP_VERTICALBAR"] = 201;
    module["scanCodes"]["KP_DBLVERTICALBAR"] = 202;
    module["scanCodes"]["KP_COLON"] = 203;
    module["scanCodes"]["KP_HASH"] = 204;
    module["scanCodes"]["KP_SPACE"] = 205;
    module["scanCodes"]["KP_AT"] = 206;
    module["scanCodes"]["KP_EXCLAM"] = 207;
    module["scanCodes"]["KP_MEMSTORE"] = 208;
    module["scanCodes"]["KP_MEMRECALL"] = 209;
    module["scanCodes"]["KP_MEMCLEAR"] = 210;
    module["scanCodes"]["KP_MEMADD"] = 211;
    module["scanCodes"]["KP_MEMSUBTRACT"] = 212;
    module["scanCodes"]["KP_MEMMULTIPLY"] = 213;
    module["scanCodes"]["KP_MEMDIVIDE"] = 214;
    module["scanCodes"]["KP_PLUSMINUS"] = 215;
    module["scanCodes"]["KP_CLEAR"] = 216;
    module["scanCodes"]["KP_CLEARENTRY"] = 217;
    module["scanCodes"]["KP_BINARY"] = 218;
    module["scanCodes"]["KP_OCTAL"] = 219;
    module["scanCodes"]["KP_DECIMAL"] = 220;
    module["scanCodes"]["KP_HEXADECIMAL"] = 221;
    module["scanCodes"]["LCTRL"] = 224;
    module["scanCodes"]["LSHIFT"] = 225;
    module["scanCodes"]["LALT"] = 226;
    module["scanCodes"]["LGUI"] = 227;
    module["scanCodes"]["RCTRL"] = 228;
    module["scanCodes"]["RSHIFT"] = 229;
    module["scanCodes"]["RALT"] = 230;
    module["scanCodes"]["RGUI"] = 231;
    module["scanCodes"]["MODE"] = 257;
    module["scanCodes"]["AUDIONEXT"] = 258;
    module["scanCodes"]["AUDIOPREV"] = 259;
    module["scanCodes"]["AUDIOSTOP"] = 260;
    module["scanCodes"]["AUDIOPLAY"] = 261;
    module["scanCodes"]["AUDIOMUTE"] = 262;
    module["scanCodes"]["MEDIASELECT"] = 263;
    module["scanCodes"]["WWW"] = 264;
    module["scanCodes"]["MAIL"] = 265;
    module["scanCodes"]["CALCULATOR"] = 266;
    module["scanCodes"]["COMPUTER"] = 267;
    module["scanCodes"]["AC_SEARCH"] = 268;
    module["scanCodes"]["AC_HOME"] = 269;
    module["scanCodes"]["AC_BACK"] = 270;
    module["scanCodes"]["AC_FORWARD"] = 271;
    module["scanCodes"]["AC_STOP"] = 272;
    module["scanCodes"]["AC_REFRESH"] = 273;
    module["scanCodes"]["AC_BOOKMARKS"] = 274;
    module["scanCodes"]["BRIGHTNESSDOWN"] = 275;
    module["scanCodes"]["BRIGHTNESSUP"] = 276;
    module["scanCodes"]["DISPLAYSWITCH"] = 277;
    module["scanCodes"]["KBDILLUMTOGGLE"] = 278;
    module["scanCodes"]["KBDILLUMDOWN"] = 279;
    module["scanCodes"]["KBDILLUMUP"] = 280;
    module["scanCodes"]["EJECT"] = 281;
    module["scanCodes"]["SLEEP"] = 282;
    module["scanCodes"]["APP1"] = 283;
    module["scanCodes"]["APP2"] = 284;
    module["scanCodes"]["AUDIOREWIND"] = 285;
    module["scanCodes"]["AUDIOFASTFORWARD"] = 286;
}

std::tuple<int, int> ScriptHost::LuaGetScreenSize() {
    int screenWidth, screenHeight;
    _engine->GetScreenSize(&screenWidth, &screenHeight);
    return std::make_tuple(screenWidth, screenHeight);
}

std::tuple<int, int> ScriptHost::LuaGetEngineVersion() { return std::make_tuple(ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR); }

std::string ScriptHost::LuaGetCpuType() { return ABYSS_BUILD_CPU; }

std::string ScriptHost::LuaGetPlatform() { return ABYSS_BUILD_PLATFORM; }

std::string ScriptHost::LuaGetBuildType() { return ABYSS_BUILD_TYPE; }

void ScriptHost::LuaPanic(std::string_view message) {
    _engine->Panic(message);
}

} // namespace AbyssEngine
