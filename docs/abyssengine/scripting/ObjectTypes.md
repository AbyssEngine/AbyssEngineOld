# Object Types

The following are object types that are defined by the system.

---

## Sprite

**Extends [Node](#node)**

Represents a sprite.

Sprites are a child class of nodes.

### Sprite Functions

#### cellSize

```lua
    function cellSize(): x, y
    function cellSize(x, y)
```

Gets/Sets the cell size of the sprite.
The cell size is the number of frames (in x by y grid) to draw at the same time.

#### bottomOrigin

```lua
    function bottomOrigin(): bool
    function bottomOrigin(isBottomOrigin)
```

Gets/Sets the indicator that the bottom left of the sprite should be where the X/Y location is oriented.

#### blendMode

```lua
    function blendMode(): string
    function blendMode(mode)
```

Gets/Sets the blend mode of the sprite.
Mode is "none", "blend", "additive", "modulate", "multiply"

#### playMode

```lua
    function playMode(): string
    function playMode(mode)
```

Gets/Sets the play mode of the sprite.
Mode is "paused", "forwards", 'backwards"

#### playLength

```lua
    function playLength(): number
    function playLength(time)
```

Gets/Sets the play length of the sprite's animation.  
Play length is the number of seconds (0.5 = half second) that it takes to run through all the frames of the current animation.

#### frame

```lua
    function frame(): number
    function frame(index)
```

Gets/Sets the current frame index for the current animation.

#### animation

```lua
    function animation(): number
    function animation(index)
```

Gets/Sets the current animation for the current  animation.  
When setting an animation, the frame index is automatically
set to 0.

#### totalAnimations

```lua
    function totalAnimations(): number
```

Gets the total number of animations available for this sprite.

#### framesPerAnimation

```lua
    function framesPerAnimation(): number
```

Gets the number of frames in an animation.

---

## Node

Represents an object on the render graph. This is the base type for all rendering objects.

### Node Functions

These are the functions exposed by the object

```lua
    function visible(): bool
    function visible(show)
```

Gets/Sets visibility of the node.
Nodes that are not visible will not be rendered (nor will the child nodes)

---

```lua
    function active(): bool
    function active(isActive)
```

Gets/Sets activate state of the node.
Nodes that are not active will neither render nor update (nor will the child nodes)

---

```lua
    function position(): x, y
    function position(x, y)
```

Gets/Sets the position of the node.

---

```lua
    function appendChild(childNode)
```

Appends the child node to this node.
