#ifndef ROPE_ROPE_HPP
#define ROPE_ROPE_HPP

#include "rope/node.hpp"

class Rope {
private:
    using Node = rope::Node;

public:
    static constexpr std::size_t maxDepth = 64;

    using Ptr = std::shared_ptr< Node >;

    Rope();
    Rope(const nstring& text);
    Rope(const Rope& other) = default;
    ~Rope() = default;
    Rope(Ptr root);

    std::string to_string() const;
    nstring to_nstring() const;
    std::size_t length() const;
    nchar operator[](std::size_t index) const;
    std::string substr(std::size_t start, std::size_t length) const;
    nstring subnstr(std::size_t start, std::size_t length) const;

    bool is_balanced() const;
    [[nodiscard]] Rope rebalance() const;

    [[nodiscard]] Rope insert(std::size_t index, const nstring& text) const;
    [[nodiscard]] Rope insert(std::size_t index, const Rope& other) const;

    [[nodiscard]] Rope append(const nstring& text) const;
    [[nodiscard]] Rope append(const Rope& other) const;

    [[nodiscard]] Rope prepend(const nstring& text) const;
    [[nodiscard]] Rope prepend(const Rope& other) const;

    [[nodiscard]] Rope erase(std::size_t start, std::size_t length) const;

    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const nstring& text) const;
    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const Rope& other) const;

    std::pair< Rope, Rope > split(std::size_t index) const;

    std::size_t find_line_start(std::size_t index) const;
    std::size_t line_count() const;
    std::size_t line_length(std::size_t line_index) const;
    std::size_t index_from_pos(std::size_t line_idx,
                               std::size_t line_pos) const;

    std::pair< std::size_t, std::size_t > pos_from_index(
        std::size_t index) const;

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