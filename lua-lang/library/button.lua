---@meta

---@class Button
---@
Button = {
    ---If true, the node is visible, otherwise it is hidden.
    ---@type boolean
    visible,

    ---If true, the node is visible and updates will be processed, otherwise it is hidden and updates will be skipped.
    ---@type boolean
    active,

    ---The caption (text) of the button.
    ---@type string
    caption = ""
}

---Removes all children from this node
function Button:removeAllChildren() end

---Removes a child from this node.
---@param child Node
function Button:removeChild(child) end

---Appends a child to this node
---@param child Node
function Button:appendChild(child) end

---Gets the node's position
---@return number x, number y
function Button:getPosition() end

---Sets the node's position
---@param x number
---@param y number
function Button:setPosition(x, y) end

---Sets the number of x/y segments for each button state.
---@param x number @ The number of x segments
---@param y number @ The number of y segments
function Button:setSegments(x, y) end

---Sets the fixed size of the button (in pixels).
---@param width number @ The width of the button
---@param height number @ The height of the button
function Button:setFixedSize(width, height) end

---Sets the text offset of the button.
---@param x number @ The x offset
---@param y number @ The y offset
function Button:setTextOffset(x, y) end

---Sets the starting index for the specified frame index.
---@param index number @ The frame index
---Value values are:
---* normal
---* pressed
---* hover
---* disabled
---* checkednormal
---* checkedhover
---* checkedpressed
---@param start number @ The starting index
function Button:setFrameIndex(frameType, index) end

---Sets the function callback for the activate (button actually clicked) event.
---@param callback function @ The callback function
function Button:onActivate(callback) end

---Sets the function callback for the pressed (mouse initially clicked) event.
---@param callback function @ The callback function
function Button:onPressed(callback) end