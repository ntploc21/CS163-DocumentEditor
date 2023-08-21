#include "rope/node_leaf.hpp"

#include <stdexcept>

namespace rope {
    Leaf::Leaf(const std::string& text) : mText(text) {
        mLength = text.length();
        mWeight = mLength;
    }

    char Leaf::operator[](std::size_t index) const {
        if (index >= mLength) throw std::out_of_range("Index out of range");
        return mText[index];
    }

    std::string Leaf::substr(std::size_t start, std::size_t length) const {
        if (start >= mLength) return "";
        length = std::min(length, mLength - start);
        return mText.substr(start, length);
    }

    std::string Leaf::to_string() const { return mText; }

    std::pair< Node::Ptr, Node::Ptr > Leaf::split(std::size_t index) const {
        return std::make_pair(std::make_shared< Leaf >(mText.substr(0, index)),
                              std::make_shared< Leaf >(mText.substr(index)));
    }

    std::vector< Node::Ptr > Leaf::leaves() const {
        return std::vector< Node::Ptr >{std::make_shared< Leaf >(*this)};
    }

}  // namespace rope