#include "rope/rope.hpp"

#include <stdexcept>

#include "rope.hpp"
#include "rope/utils.hpp"

using namespace rope;

Rope::Rope() : Rope{""} {}

Rope::Rope(const nstring& text) : mRoot{std::make_shared< Leaf >(text)} {}

Rope::Rope(Ptr root) : mRoot{std::move(root)} {}

std::string Rope::to_string() const { return mRoot->to_string(); }

nstring Rope::to_nstring() const { return mRoot->to_nstring(); }

std::size_t Rope::length() const { return mRoot->length(); }

nchar Rope::operator[](std::size_t index) const {
    if (index > length()) throw std::out_of_range("Index out of range");
    if (index == length()) return '\0';
    return mRoot->operator[](index);
}

std::string Rope::substr(std::size_t start, std::size_t length) const {
    return mRoot->substr(start, length);
}

nstring Rope::subnstr(std::size_t start, std::size_t length) const {
    return mRoot->subnstr(start, length);
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

Rope Rope::insert(std::size_t index, const nstring& text) const {
    return insert(index, Rope(text));
}

Rope Rope::insert(std::size_t index, const Rope& other) const {
    index = std::min(index, length());
    if (index == length()) return append(other);
    if (index == 0) return other.append(*this);

    auto [left, right] = split(index);
    return left.append(other).append(right);
}

Rope Rope::append(const nstring& text) const { return append(Rope(text)); }

Rope Rope::append(const Rope& other) const {
    return Rope(std::make_shared< Concatenation >(mRoot, other.mRoot));
}

Rope Rope::prepend(const nstring& text) const { return prepend(Rope(text)); }

Rope Rope::prepend(const Rope& other) const {
    return Rope(std::make_shared< Concatenation >(other.mRoot, mRoot));
}

Rope Rope::erase(std::size_t start, std::size_t length) const {
    auto [left, right] = split(start);
    auto [middle, _] = right.split(length);
    return left.append(_);
}

Rope Rope::replace(std::size_t start, std::size_t length,
                   const nstring& text) const {
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

std::size_t Rope::find_word_start(std::size_t word_index) const {
    if (word_index == 0) return 0;
    if (word_index >= word_count()) return length();
    return mRoot->find_word_start(word_index);
}

std::size_t Rope::find_word_at(std::size_t index) const {
    if (index == 0) return 0;
    if (index >= length()) return word_count();
    return mRoot->find_word_at(index);
}

std::size_t Rope::word_count() const {
    return mRoot->word_count() +
           (length() ? mRoot->operator[](length() - 1) != ' ' : 0);
}

std::size_t Rope::find_line_start(std::size_t index) const {
    if (index == 0) return 0;
    if (index >= line_count()) return length();
    return mRoot->find_line_feed(index - 1) + 1;
}

std::size_t Rope::line_count() const {
    return mRoot->line_count() +
           (length() ? mRoot->operator[](length() - 1) != '\n' : 0);
}

std::size_t Rope::line_length(std::size_t line_index) const {
    if (line_index == line_count() - 1) {
        std::size_t end = length() - 1;
        while (end > 0 && mRoot->operator[](end) == '\n') end--;
        return length() - mRoot->find_line_start(line_index) -
               (length() - end - 1);
    }
    return mRoot->find_line_start(line_index + 1) -
           mRoot->find_line_start(line_index) - 1;
}

std::size_t Rope::index_from_pos(std::size_t line_idx,
                                 std::size_t line_pos) const {
    return find_line_start(line_idx) + line_pos;
}
std::pair< std::size_t, std::size_t > Rope::pos_from_index(
    std::size_t index) const {
    return mRoot->pos_from_index(index);
}