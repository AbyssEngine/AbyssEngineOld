# Engine Functions

These are the functions available from lua in the Abyss Engine scripting environment

---

## getConfig

```lua
function getConfig(category, value): string
```

Returns the value of a configuration element.
    Category/Value is based on the INI file.

A system category "#Abyss" has the following properties:

`"BasePath" - The base path of the engine`

### Example

```lua
function testConfigGet()
    log("info", getConfig("Foo", "Bar"))
end
```

```ini
[Foo]
Bar=test
```

Would return `"test"`

---

## log

```lua
function log(level, message)
```

Writes message to the log.

**!!NOTE!!** Loglevel is INFO in release builds, bear this in mind if you need to display useful info in a log

`level: Log severity level, valid values are: "info", "error", "fatal", "warn", "debug", "trace"`

`message: Message to be printed`

### Example
```lua
function test()
    log("info", "Foo Bar")
end
```

---

## showSystemCursor

```lua
function showSystemCursor(show)
```
Shows/hides the system cursor

`show: Boolean value for showing/hiding system cursor`
