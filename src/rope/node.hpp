#ifndef ROPE_NODE_HPP
#define ROPE_NODE_HPP

#include <memory>
#include <string>
#include <vector>

// Currently use std::string for storing text, after we reimplement a new char
// type, we will plug it in here.

namespace rope {

    class Node {
    public:
        using Ptr = std::shared_ptr< Node >;

        virtual char operator[](std::size_t index) const = 0;
        virtual std::string substr(std::size_t start,
                                   std::size_t length) const = 0;
        virtual std::string to_string() const = 0;
        virtual std::pair< Ptr, Ptr > split(std::size_t index) const = 0;
        virtual std::vector< Ptr > leaves() const = 0;
        virtual ~Node() = default;

        std::size_t length() const;
        std::size_t depth() const;

    protected:
        std::size_t mDepth{};

        std::size_t mLength{};
        std::size_t mWeight{};
    };

}  // namespace rope

#endif  // ROPE_NODE_HPP