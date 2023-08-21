#ifndef ROPE_NODE_CONCATENATION_HPP
#define ROPE_NODE_CONCATENATION_HPP

#include "rope/node.hpp"

namespace rope {

    class Concatenation : public Node {
    public:
        Concatenation(const Node::Ptr& left, const Node::Ptr& right);
        Concatenation(const Concatenation& other);
        ~Concatenation() override = default;

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

        Node::Ptr mLeft{};
        Node::Ptr mRight{};
    };

}  // namespace rope

#endif  // ROPE_NODE_CONCATENATION_HPP