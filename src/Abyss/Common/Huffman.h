#pragma once

#include <memory>

namespace Abyss::Common {

class Huffman {
  private:
    class LinkedNode : public std::enable_shared_from_this<LinkedNode> {
      private:
        int _decompressedValue;
        int _weight;
        std::shared_ptr<LinkedNode> _parent;
        std::shared_ptr<LinkedNode> _firstChild;
        std::shared_ptr<LinkedNode> _prevSibling;
        std::shared_ptr<LinkedNode> _nextSibling;

      public:
        LinkedNode(int decompressedValue, int weight);
        [[nodiscard]] auto getDecompressedValue() const -> int;
        [[nodiscard]] auto getWeight() const -> int;
        [[nodiscard]] auto getParent() const -> std::shared_ptr<LinkedNode>;
        [[nodiscard]] auto getFirstChild() -> std::shared_ptr<LinkedNode>;
        [[nodiscard]] auto getPrevSibling() const -> std::shared_ptr<LinkedNode>;
        [[nodiscard]] auto getNextSibling() const -> std::shared_ptr<LinkedNode>;
        [[nodiscard]] auto getChild1() const -> std::shared_ptr<LinkedNode>;
        auto setNextSibling(const std::shared_ptr<LinkedNode> &nextSibling) -> void;
        auto setPrevSibling(const std::shared_ptr<LinkedNode> &prevSibling) -> void;
        auto insert(std::shared_ptr<LinkedNode> other) -> std::shared_ptr<LinkedNode>;
    };
};
} // namespace Abyss::Common
