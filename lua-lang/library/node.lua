---@meta

---@class Node
Node = {
    ---If true, the node is visible, otherwise it is hidden.
    ---@type boolean
    visible = true,

    ---If true, the node is visible and updates will be processed, otherwise it is hidden and updates will be skipped.
    ---@type boolean
    active = true
}

---Removes all children from this node
function Node:removeAllChildren() end

---Removes a child from this node.
---@param child Node
function Node:removeChild(child) end

---Appends a child to this node
---@param child Node
function Node:appendChild(child) end

---Gets the node's position
---@return number x, number y
function Node:getPosition() end

---Sets the node's position
---@param x number
---@param y number
function Node:setPosition(x, y) end