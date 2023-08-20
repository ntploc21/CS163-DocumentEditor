#ifndef ROPE_NODE_LEAF_HPP
#define ROPE_NODE_LEAF_HPP

#include "rope/node.hpp"

namespace rope {

    class Leaf : public Node {
    public:
        Leaf(const std::string& text);
        ~Leaf() override = default;

        char operator[](std::size_t index) const override;
        std::string substr(std::size_t start,
                           std::size_t length) const override;
        std::string to_string() const override;
        std::pair< Node::Ptr, Node::Ptr > split(
            std::size_t index) const override;
        std::vector< Node::Ptr > leaves() const override;

    private:
        using Node::mDepth;

        using Node::mLength;
        using Node::mWeight;

        std::string mText;
    };

}  // namespace rope

#endif  // ROPE_NODE_LEAF_HPP