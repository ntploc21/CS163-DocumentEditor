#ifndef ROPE_ROPE_HPP
#define ROPE_ROPE_HPP

#include "rope/node.hpp"
#include "rope/node_concatenation.hpp"
#include "rope/node_leaf.hpp"

class Rope {
private:
    using Node = rope::Node;

public:
    static constexpr std::size_t maxDepth = 64;

    using Ptr = std::shared_ptr< Node >;

    Rope();
    Rope(const std::string& text);
    Rope(const Rope& other) = default;
    Rope(Ptr root);

    std::string to_string() const;
    std::size_t length() const;
    char operator[](std::size_t index) const;
    std::string substr(std::size_t start, std::size_t length) const;

    bool is_balanced() const;
    [[nodiscard]] Rope rebalance() const;

    [[nodiscard]] Rope insert(std::size_t index, const std::string& text) const;
    [[nodiscard]] Rope insert(std::size_t index, const Rope& other) const;

    [[nodiscard]] Rope append(const std::string& text) const;
    [[nodiscard]] Rope append(const Rope& other) const;

    [[nodiscard]] Rope prepend(const std::string& text) const;
    [[nodiscard]] Rope prepend(const Rope& other) const;

    [[nodiscard]] Rope erase(std::size_t start, std::size_t length) const;

    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const std::string& text) const;
    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const Rope& other) const;

    std::pair< Rope, Rope > split(std::size_t index) const;

    Rope& operator=(const Rope& other) = default;
    bool operator==(const Rope& other) const;
    bool operator!=(const Rope& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Rope& rope);

private:
    Ptr mRoot{};

    static Node::Ptr merge(const std::vector< Node::Ptr >& leaves,
                           std::size_t left, std::size_t right);

    static Rope merge(const std::vector< Node::Ptr >& leaves);
};

#endif  // ROPE_ROPE_HPP