#include "scripthost.h"
#include "../hostnotify/hostnotify.h"
#include "../node/d2rsprite.h"
#include "../node/dc6sprite.h"
#include "cascprovider.h"
#include "engine.h"
#include "filesystemprovider.h"
#include "mpqprovider.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <filesystem>
#include <memory>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

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

AbyssEngine::ScriptHost::ScriptHost(Engine *engine) : _lua(), _engine(engine) {
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
    module.set_function("createLabel", &ScriptHost::LuaCreateLabel, this);
    module.set_function("createPalette", &ScriptHost::LuaCreatePalette, this);
    module.set_function("createSoundEffect", &ScriptHost::LuaCreateSoundEffect, this);
    module.set_function("createSprite", &ScriptHost::LuaCreateSprite, this);
    module.set_function("createSpriteFont", &ScriptHost::LuaCreateSpriteFont, this);
    module.set_function("createString", &ScriptHost::LuaCreateText, this);
    module.set_function("fileExists", &ScriptHost::LuaFileExists, this);
    module.set_function("getConfig", &ScriptHost::LuaGetConfig, this);
    module.set_function("getRootNode", &ScriptHost::LuaGetRootNode, this);
    module.set_function("log", &ScriptHost::LuaLog, this);
    module.set_function("playBackgroundMusic", &ScriptHost::LuaPlayBackgroundMusic, this);
    module.set_function("playVideo", &ScriptHost::LuaPlayVideo, this);
    module.set_function("resetMouseState", &ScriptHost::LuaResetMouseState, this);
    module.set_function("setCursor", &ScriptHost::LuaSetCursor, this);
    module.set_function("showSystemCursor", &ScriptHost::LuaShowSystemCursor, this);
    module.set_function("shutdown", &ScriptHost::LuaFuncShutdown, this);

    // User Types -------------------------------------------------------------------------------------------------------------------------

    // SpriteFont (Not node based)
    module.new_usertype<SpriteFont>("SpriteFont", sol::no_constructor);

    // Button
    auto buttonType = CreateLuaObjectType<Button>(module, "Button", sol::no_constructor);
    buttonType["setSegments"] = &Button::SetSegments;
    buttonType["setFixedSize"] = &Button::SetSize;
    buttonType["caption"] = sol::property(&Button::GetCaption, &Button::SetCaption);
    buttonType["setTextOffset"] = &Button::SetTextOffset;
    buttonType["setFrameIndex"] = &Button::LuaSetFrameIndex;
    buttonType["onActivate"] = &Button::LuaSetActivateCallback;
    buttonType["onPressed"] = &Button::LuaSetPressCallback;

    // Node
    auto nodeType = module.new_usertype<Node>("Node", sol::no_constructor);
    BindNodeFunctions(nodeType);

    // Label
    auto labelType = CreateLuaObjectType<Label>(module, "Label", sol::no_constructor);
    labelType["setCaption"] = &Label::SetCaption;
    labelType["setAlignment"] = &Label::SetAlignmentStr;
    labelType["setColorMod"] = &Label::SetColorMod;

    // Sprite
    auto spriteType = CreateLuaObjectType<Sprite>(module, "Sprite", sol::no_constructor);
    spriteType["setCellSize"] = &Sprite::SetCellSize;
    spriteType["getCellSize"] = &Sprite::GetCellSize;
    spriteType["onMouseButtonDown"] = &Sprite::SetLuaMouseButtonDownHandler;
    spriteType["onMouseButtonUp"] = &Sprite::SetLuaMouseButtonUpHandler;
    spriteType["blendMode"] = sol::property(&Sprite::LuaGetBlendMode, &Sprite::LuaSetBlendMode);
    spriteType["bottomOrigin"] = sol::property(&Sprite::GetIsBottomOrigin, &Sprite::SetIsBottomOrigin);
    spriteType["playMode"] = sol::property(&Sprite::LuaGetPlayMode, &Sprite::LuaSetPlayMode);

    // Sound Effect
    auto soundEffect =  module.new_usertype<SoundEffect>("SoundEffect", sol::no_constructor);
    soundEffect["play"] = &SoundEffect::Play;
    soundEffect["stop"] = &SoundEffect::Stop;
    soundEffect["pause"] = &SoundEffect::Pause;
    soundEffect["isPlaying"] = &SoundEffect::GetIsPlaying;
    soundEffect["isPaused"] = &SoundEffect::GetIsPaused;
    soundEffect["volume"] = sol::property(&SoundEffect::GetVolume, &SoundEffect::SetVolume);
    soundEffect["loop"] = sol::property(&SoundEffect::GetLoop, &SoundEffect::SetLoop);

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
        SPDLOG_ERROR("Error loading file {0}: {1}", path.string(), e.what());
        HostNotify::Notify(eNotifyType::Fatal, "Script Error", e.what());
        _engine->Stop();
        return std::tuple<sol::nil_t, sol::nil_t>();
    }

    sol::unsafe_function func = std::get<0>(ret);
    return func();
}

void AbyssEngine::ScriptHost::ExecuteString(std::string_view code) {
    auto result = _lua.script(code, _environment);
    if (!result.valid()) {
        const sol::error e = result;
        SPDLOG_ERROR("Script Error: {0}", e.what());
        HostNotify::Notify(eNotifyType::Fatal, "Script Error", e.what());
        _engine->Stop();
        return;
    }
}

sol::object AbyssEngine::ScriptHost::LuaDoFile(std::string_view path) {
    std::tuple<sol::object, sol::object> ret = LuaLoadFile(path);
    if (std::get<0>(ret) == sol::nil) {
        auto e = sol::error(std::get<1>(ret).as<std::string>());
        SPDLOG_ERROR("Script Error: {0}", e.what());
        HostNotify::Notify(eNotifyType::Fatal, "Script Error", e.what());
        _engine->Stop();
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

std::unique_ptr<AbyssEngine::Sprite> AbyssEngine::ScriptHost::LuaCreateSprite(std::string_view spritePath, std::string_view paletteName = "") {
    const auto &engine = _engine;
    const std::filesystem::path path(spritePath);

    if (!engine->GetLoader().FileExists(path))
        throw std::runtime_error("File not found: " + std::string(spritePath));

    auto stream = engine->GetLoader().Load(path);

    std::string lower = absl::AsciiStrToLower(spritePath);
    if (lower.ends_with(".dc6")) {
        const auto &palette = engine->GetPalette(paletteName);
        return std::make_unique<DC6Sprite>(spritePath, stream, palette);
    } else if (lower.ends_with(".sprite")) {
        return std::make_unique<D2RSprite>(stream);
    } else
        throw std::runtime_error(absl::StrCat("Unknowns sprite format for file: ", spritePath));
}

std::unique_ptr<AbyssEngine::Button> AbyssEngine::ScriptHost::LuaCreateButton(SpriteFont *spriteFont, Sprite *sprite) {
    return std::make_unique<Button>(spriteFont, sprite);
}

void AbyssEngine::ScriptHost::LuaSetCursor(Sprite &sprite, int offsetX, int offsetY) { _engine->SetCursorSprite(&sprite, offsetX, offsetY); }

AbyssEngine::Node &AbyssEngine::ScriptHost::LuaGetRootNode() { return _engine->GetRootNode(); }

void AbyssEngine::ScriptHost::LuaPlayVideo(std::string_view videoPath, bool wait) {
    auto stream = _engine->GetLoader().Load(std::filesystem::path(videoPath));
    _engine->PlayVideo(videoPath, std::move(stream), wait);
    if (wait)
        _engine->WaitForVideoToFinish();
}

template <class T, typename X>
sol::basic_usertype<T, sol::basic_reference<false>> AbyssEngine::ScriptHost::CreateLuaObjectType(sol::table &module, std::string_view name,
                                                                                                 X &&constructor) {
    auto val = module.new_usertype<T>(name, "new", std::forward<X>(constructor), sol::base_classes, sol::bases<Node>());
    BindNodeFunctions(val);
    return val;
}

template <class T> void AbyssEngine::ScriptHost::BindNodeFunctions(sol::basic_usertype<T, sol::basic_reference<false>> &val) {
    val["removeAllChildren"] = &T::RemoveAllChildren;
    val["appendChild"] = &T::AppendChild;
    val["removeChild"] = &T::RemoveChild;
    val["getPosition"] = &T::GetPosition;
    val["setPosition"] = &T::SetPosition;
    val["visible"] = sol::property(&T::GetVisible, &T::SetVisible);
    val["active"] = sol::property(&T::GetActive, &T::SetActive);
}

void AbyssEngine::ScriptHost::LuaResetMouseState() { _engine->ResetMouseButtonState(); }

std::string AbyssEngine::ScriptHost::LuaCreateText(std::string_view filePath) {
    if (!_engine->GetLoader().FileExists(filePath))
        throw std::runtime_error(absl::StrCat("Path does not exist: ", filePath));

    auto stream = _engine->GetLoader().Load(filePath);
    auto result = std::string();
    result.resize(stream.size());
    stream.read(result.data(), stream.size());
    return result;
}

std::unique_ptr<AbyssEngine::SpriteFont> AbyssEngine::ScriptHost::LuaCreateSpriteFont(std::string_view fontPath, std::string_view paletteName) {
    return std::make_unique<SpriteFont>(fontPath, paletteName);
}

std::unique_ptr<AbyssEngine::Label> AbyssEngine::ScriptHost::LuaCreateLabel(AbyssEngine::SpriteFont *spriteFont) {
    return std::make_unique<AbyssEngine::Label>(spriteFont);
}

void AbyssEngine::ScriptHost::GC() { _lua.collect_garbage(); }

void AbyssEngine::ScriptHost::LuaPlayBackgroundMusic(std::string_view fileName) {
    auto& loader = _engine->GetLoader();

    if (!loader.FileExists(fileName))
        throw std::runtime_error(absl::StrCat("File not found: ", fileName));

    auto stream = loader.Load(fileName);
    auto result = std::make_unique<LibAbyss::AudioStream>(std::move(stream));
    result->SetLoop(true);
    result->Play();
    _engine->GetSystemIO().SetBackgroundMusic(std::move(result));
}

std::unique_ptr<AbyssEngine::SoundEffect> AbyssEngine::ScriptHost::LuaCreateSoundEffect(std::string_view fileName) {
    auto& loader = _engine->GetLoader();

    if (!loader.FileExists(fileName))
        throw std::runtime_error(absl::StrCat("File not found: ", fileName));

    auto stream = loader.Load(fileName);
    auto audioStream = std::make_unique<LibAbyss::AudioStream>(std::move(stream));

    return std::make_unique<SoundEffect>(std::move(audioStream));
}
