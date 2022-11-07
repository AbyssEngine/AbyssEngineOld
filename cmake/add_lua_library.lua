-- This implements a workaround for https://github.com/luarocks/luarocks/issues/262
-- invoked from Lualibs.cmake
-- works only for a limited subset of rocks

dir = arg[1]
lib = arg[2]
ver = arg[3]

rockspec = dir .. '/' .. lib .. '-' .. ver .. '/' .. lib .. '-' .. ver .. '.rockspec'
env = {}
func = loadfile(rockspec, 't', env)
if _G.setfenv then
    -- for lua 5.1
    setfenv(func, env)
end
func()

assert(env.build.type == 'builtin', 'build.type is "' .. env.build.type .. '", not "builtin"')

if env.source.file == nil then
    zipname = string.gsub(env.source.url, '.*/', '')
else
    zipname = env.source.file
end
zipname = string.gsub(zipname, '.zip', '')
zipname = string.gsub(zipname, '.tar.gz', '')
filebase = dir .. '/' .. lib .. '-' .. ver .. '/' .. zipname .. '/'
desc = env.build.modules[lib]
if type(desc) == 'string' then
    desc = { sources = { desc } }
else
    if type(desc) == 'table' and type(desc[1]) == 'string' then
        desc = { sources = desc }
    end
end

text = 'add_library(LUA_LIB_NAME STATIC)'
text = string.gsub(text, 'NAME', lib)
print(text)

text = 'add_library(LUA::NAME ALIAS LUA_LIB_NAME)'
text = string.gsub(text, 'NAME', lib)
print(text)

for i, v in ipairs(desc.sources) do
    text = 'target_sources(LUA_LIB_NAME PRIVATE FILE)'
    text = string.gsub(text, 'NAME', lib)
    text = string.gsub(text, 'FILE', filebase .. v)
    print(text)
end

if desc.defines then
    for i, v in ipairs(desc.defines) do
        text = 'target_compile_definitions(LUA_LIB_NAME PRIVATE DEF)'
        text = string.gsub(text, 'NAME', lib)
        text = string.gsub(text, 'DEF', v)
        print(text)
    end
end

text = 'target_link_libraries(LUA_LIB_NAME PRIVATE PkgConfig::Lua)'
text = string.gsub(text, 'NAME', lib)
print(text)

if desc.libraries then
    for i, v in ipairs(desc.libraries) do
        text = 'pkg_check_modules(LUA_NAME_DEP_DEPEND REQUIRED IMPORTED_TARGET DEPEND)'
        text = string.gsub(text, 'NAME', lib)
        text = string.gsub(text, 'DEPEND', v)
        print(text)

        text = 'target_link_libraries(LUA_LIB_NAME PUBLIC PkgConfig::LUA_NAME_DEP_DEPEND)'
        text = string.gsub(text, 'NAME', lib)
        text = string.gsub(text, 'DEPEND', v)
        print(text)
    end
end
