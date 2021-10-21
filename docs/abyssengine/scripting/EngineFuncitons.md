# Engine Functions

These are the functions available from lua in the Abyss Engine scripting environment

---

```lua
function getConfig(category, value): string
    --[[
    Returns the value of a configuration element.
    Category/Value is based on the INI file.

    A system category "#Abyss" has the following properties:
        "BasePath" - The base path of the engine
    ]]--
-- Example:
function testConfigGet()
    log("info", getConfig("Foo", "Bar"))
end
```

---

```lua
function log(level, message)
    --[[
    Writes message to the log
    Level is "info", "error", "fatal", "warn", debug", "trace"
    "info" does not show on release mode builds of the engine.
    ]]--
-- Example: 
function test()
    log("info", "Foo Bar")
end
```

---

```lua
function showSystemCursor(show)
    Shows/hides the system cursor
```