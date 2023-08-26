#include <algorithm>
#include <stdexcept>

#include "node.hpp"
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

    std::pair< std::size_t, std::size_t > Leaf::pos_from_index(
        std::size_t index) const {
        index = std::min(index, mLength);

        std::size_t line_idx =
            lower_bound(mLinePos.begin(), mLinePos.end(), (int)index) -
            mLinePos.begin();

        std::size_t pos_idx = index;
        if (line_idx) pos_idx -= mLinePos[line_idx - 1] + 1;

        return std::make_pair(line_idx, pos_idx);
    }
    std::size_t Leaf::find_line_feed(std::size_t index) const {
        if (mLineCount) index = std::min(index, mLineCount - 1);

        if (mLineCount == 0) return 0;

        if (index >= mLineCount) throw std::out_of_range("Index out of range");

        return mLinePos.at(index);
    }

    std::size_t Leaf::find_word_start(std::size_t index) const {
        if (index >= mWordCount) throw std::out_of_range("Index out of range");
        return mWordPos.at(index);
    }

    std::size_t Leaf::find_word_at(std::size_t index) const {
        if (index >= mLength) throw std::out_of_range("Index out of range");

        if (index == 0) return 0;

        // if (index == mLength - 1) {
        //     if (mText[index] != ' ' && mText[index] != '\n' &&
        //         mText[index] != '\t')
        //         return 1;
        //     return 0;
        // }

        std::size_t word_index =
            upper_bound(mWordPos.begin(), mWordPos.end(), (int)index) -
            mWordPos.begin();

        std::cout << word_index << " _ " << index << std::endl;

        if (word_index == 0) return 0;
        return word_index - 1;
    }

    std::size_t Leaf::line_count() const { return mLineCount; }

    std::size_t Leaf::word_count() const { return mWordCount; }

}  // namespace rope