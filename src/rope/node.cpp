#include "rope/node.hpp"

namespace rope {
    std::size_t Node::length() const { return mLength; }

    std::size_t Node::depth() const { return mDepth; }

}  // namespace rope