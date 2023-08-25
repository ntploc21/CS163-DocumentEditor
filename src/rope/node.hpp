#ifndef ROPE_NODE_HPP
#define ROPE_NODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "text/nstring.hpp"

// Currently use std::string for storing text, after we reimplement a new char
// type, we will plug it in here.

namespace rope {

    class Node {
    public:
        using Ptr = std::shared_ptr< Node >;

        virtual std::string substr(std::size_t start,
                                   std::size_t length) const = 0;
        virtual std::string to_string() const = 0;

        virtual nchar operator[](std::size_t index) const = 0;
        virtual nstring subnstr(std::size_t start,
                                std::size_t length) const = 0;
        virtual nstring to_nstring() const = 0;

        virtual std::pair< Ptr, Ptr > split(std::size_t index) const = 0;
        virtual std::vector< Ptr > leaves() const = 0;
        virtual ~Node() = default;

        virtual std::pair< std::size_t, std::size_t > pos_from_index(
            std::size_t index) const = 0;

        virtual std::size_t find_line_feed(std::size_t index) const = 0;
        virtual std::size_t find_line_start(std::size_t line_index) const;

        virtual std::size_t find_word_start_(std::size_t word_index) const = 0;
        virtual std::size_t find_word_start(std::size_t index) const;

        std::size_t length() const;
        std::size_t depth() const;

        virtual std::size_t line_count() const = 0;
        virtual std::size_t word_count() const = 0;

    protected:
        std::size_t mDepth{};

        std::size_t mLength{};
        std::size_t mWeight{};

        std::size_t mWordCount{};
        std::size_t mWordWeight{};

        std::size_t mLineCount{};
        std::size_t mLineWeight{};
    };

    class Concatenation : public Node {
    public:
        Concatenation(Node::Ptr left, Node::Ptr right);
        Concatenation(const Concatenation& other);
        ~Concatenation() override = default;

        std::string substr(std::size_t start,
                           std::size_t length) const override;
        std::string to_string() const override;

        nchar operator[](std::size_t index) const override;
        nstring subnstr(std::size_t start, std::size_t length) const override;
        nstring to_nstring() const override;

        std::pair< Node::Ptr, Node::Ptr > split(
            std::size_t index) const override;
        std::vector< Node::Ptr > leaves() const override;

        std::pair< std::size_t, std::size_t > pos_from_index(
            std::size_t index) const override;

        std::size_t find_line_feed(std::size_t index) const override;
        std::size_t find_word_start_(std::size_t index) const override;

        std::size_t line_count() const override;
        std::size_t word_count() const override;

    private:
        using Node::mDepth;

        using Node::mLength;
        using Node::mWeight;

        using Node::mWordCount;
        using Node::mWordWeight;

        using Node::mLineCount;
        using Node::mLineWeight;

        Node::Ptr mLeft{};
        Node::Ptr mRight{};
    };

    class Leaf : public Node {
    public:
        Leaf(const nstring& text);
        ~Leaf() override = default;

        std::string substr(std::size_t start,
                           std::size_t length) const override;
        std::string to_string() const override;

        nchar operator[](std::size_t index) const override;
        nstring subnstr(std::size_t start, std::size_t length) const override;
        nstring to_nstring() const override;

        std::pair< Node::Ptr, Node::Ptr > split(
            std::size_t index) const override;
        std::vector< Node::Ptr > leaves() const override;

        std::pair< std::size_t, std::size_t > pos_from_index(
            std::size_t index) const override;

        std::size_t find_line_feed(std::size_t index) const override;
        std::size_t find_word_start_(std::size_t index) const override;

        std::size_t line_count() const override;
        std::size_t word_count() const override;

    private:
        using Node::mDepth;

        using Node::mLength;
        using Node::mWeight;

        using Node::mWordCount;
        using Node::mWordWeight;

        using Node::mLineCount;
        using Node::mLineWeight;

        nstring mText{};

        std::vector< int > mLinePos{};
        std::vector< int > mWordPos{};
    };

}  // namespace rope

#endif  // ROPE_NODE_HPP