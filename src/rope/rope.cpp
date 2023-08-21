#include "rope/rope.hpp"

#include <stdexcept>

#include "rope.hpp"
#include "rope/utils.hpp"

using namespace rope;

Rope::Rope() : Rope("") {}

Rope::Rope(const std::string& text) : mRoot(std::make_shared< Leaf >(text)) {}

Rope::Rope(Ptr root) : mRoot(std::move(root)) {}

std::string Rope::to_string() const { return mRoot->to_string(); }

std::size_t Rope::length() const { return mRoot->length(); }

char Rope::operator[](std::size_t index) const {
    if (index > length()) throw std::out_of_range("Index out of range");
    if (index == length()) return '\0';
    return mRoot->operator[](index);
}

std::string Rope::substr(std::size_t start, std::size_t length) const {
    return mRoot->substr(start, length);
}

// will implement later
bool Rope::is_balanced() const {
    if (mRoot->depth() >= Rope::maxDepth - 2) return false;
    return mRoot->length() >= fib(mRoot->depth() + 2);
}

// will implement later
Rope Rope::rebalance() const {
    if (is_balanced()) return *this;

    auto leaves = mRoot->leaves();
    return merge(leaves);
}

Rope Rope::insert(std::size_t index, const std::string& text) const {
    return insert(index, Rope(text));
}

Rope Rope::insert(std::size_t index, const Rope& other) const {
    index = std::min(index, length());
    if (index == length()) return append(other);
    if (index == 0) return other.append(*this);

    auto [left, right] = split(index);
    return left.append(other).append(right);
}

Rope Rope::append(const std::string& text) const { return append(Rope(text)); }

Rope Rope::append(const Rope& other) const {
    return Rope(std::make_shared< Concatenation >(mRoot, other.mRoot));
}

Rope Rope::prepend(const std::string& text) const {
    return prepend(Rope(text));
}

Rope Rope::prepend(const Rope& other) const {
    return Rope(std::make_shared< Concatenation >(other.mRoot, mRoot));
}

Rope Rope::erase(std::size_t start, std::size_t length) const {
    auto [left, right] = split(start);
    auto [middle, _] = right.split(length);
    return left.append(_);
}

Rope Rope::replace(std::size_t start, std::size_t length,
                   const std::string& text) const {
    return replace(start, length, Rope(text));
}

Rope Rope::replace(std::size_t start, std::size_t length,
                   const Rope& other) const {
    return erase(start, length).insert(start, other);
}

std::pair< Rope, Rope > Rope::split(std::size_t index) const {
    auto [left, right] = mRoot->split(index);
    return std::make_pair(Rope(left), Rope(right));
}

Rope::Ptr Rope::merge(const std::vector< Rope::Ptr >& leaves, std::size_t left,
                      std::size_t right) {
    if (left == right) return leaves[left];

    auto middle = (left + right) / 2;
    return std::make_shared< Concatenation >(merge(leaves, left, middle),
                                             merge(leaves, middle + 1, right));
}

Rope Rope::merge(const std::vector< Rope::Ptr >& leaves) {
    return Rope(merge(leaves, 0, leaves.size() - 1));
}

bool Rope::operator==(const Rope& other) const {
    return mRoot->to_string() == other.mRoot->to_string();
}

bool Rope::operator!=(const Rope& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Rope& rope) {
    return os << rope.to_string();
}