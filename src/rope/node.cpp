#include "rope/node.hpp"

namespace rope {
    std::size_t Node::length() const { return mLength; }

    std::size_t Node::depth() const { return mDepth; }

    std::size_t Node::line_count() const { return mLineCount; }

    std::size_t Node::word_count() const { return mWordCount; }

}  // namespace rope