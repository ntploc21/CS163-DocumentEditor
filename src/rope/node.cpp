#include "rope/node.hpp"

namespace rope {
    std::size_t Node::find_line_start(std::size_t line_index) const {
        if (line_index == 0) return 0;

        return find_line_feed(line_index - 1) + 1;
    }

    std::size_t Node::find_word_start(std::size_t index) const {
        if (index == 0) return 0;

        return find_word_start_(index - 1);
    }

    std::size_t Node::length() const { return mLength; }

    std::size_t Node::depth() const { return mDepth; }

    std::size_t Node::line_count() const { return mLineCount; }

    std::size_t Node::word_count() const { return mWordCount; }

}  // namespace rope