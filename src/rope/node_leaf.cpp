#include <stdexcept>

#include "rope/node.hpp"

namespace rope {
    Leaf::Leaf(const nstring& text) : mText{text} {
        mLength = text.length();
        mWeight = mLength;

        for (std::size_t i = 0; i < mLength; ++i) {
            if (mText[i] == '\n') {
                mLinePos.push_back(i);
            }
        }
        mLineCount = mLineWeight = mLinePos.size();

        // count word in a string
        nchar prv = "";
        for (std::size_t i = 0; i < mLength; ++i) {
            if (mText[i] != '\n' && mText[i] != ' ' && mText[i] != '\t') {
                if (prv == '\n' || prv == ' ' || prv == '\t') {
                    mWordPos.push_back(i);
                }
            }
            prv = mText[i];
        }
        mWordCount = mWordWeight = mWordPos.size();
    }

    std::string Leaf::substr(std::size_t start, std::size_t length) const {
        return subnstr(start, length).to_string();
    }

    std::string Leaf::to_string() const { return mText.to_string(); }
    nstring Leaf::to_nstring() const { return mText; }

    nchar Leaf::operator[](std::size_t index) const {
        if (index >= mLength) throw std::out_of_range("Index out of range");
        return mText[index];
    }

    nstring Leaf::subnstr(std::size_t start, std::size_t length) const {
        if (start >= mLength) return "";
        length = std::min(length, mLength - start);
        return mText.substr(start, length);
    }

    std::pair< Node::Ptr, Node::Ptr > Leaf::split(std::size_t index) const {
        return std::make_pair(std::make_shared< Leaf >(mText.substr(0, index)),
                              std::make_shared< Leaf >(mText.substr(index)));
    }

    std::vector< Node::Ptr > Leaf::leaves() const {
        return std::vector< Node::Ptr >{std::make_shared< Leaf >(*this)};
    }

    std::size_t Leaf::find_line_start(std::size_t index) const {
        if (index >= mLineCount) throw std::out_of_range("Index out of range");
        return mLinePos.at(index);
    }

    std::size_t Leaf::find_word_start(std::size_t index) const {
        if (index >= mWordCount) throw std::out_of_range("Index out of range");
        return mWordPos.at(index);
    }

    std::size_t Leaf::line_count() const { return mLineCount; }

    std::size_t Leaf::word_count() const { return mWordCount; }

}  // namespace rope