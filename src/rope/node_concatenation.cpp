#include <cassert>

#include "node.hpp"
#include "rope/node.hpp"

namespace rope {
    Concatenation::Concatenation(Node::Ptr left, Node::Ptr right)
        : mLeft{std::move(left)}, mRight{std::move(right)} {
        mWeight = mLeft ? mLeft->length() : 0;
        mLength = mWeight + (mRight ? mRight->length() : 0);

        mLineWeight = mLeft ? mLeft->line_count() : 0;
        mLineCount = mLineWeight + (mRight ? mRight->line_count() : 0);

        mWordWeight = mLeft ? mLeft->word_count() : 0;
        mWordCount = mWordWeight + (mRight ? mRight->word_count() : 0);

        std::size_t leftDepth = mLeft ? mLeft->depth() : 0;
        std::size_t rightDepth = mRight ? mRight->depth() : 0;
        mDepth = std::max(leftDepth, rightDepth) + 1;
    }

    Concatenation::Concatenation(const Concatenation& other)
        : Concatenation{other.mLeft, other.mRight} {}

    nchar Concatenation::operator[](std::size_t index) const {
        assert(index < mLength);
        if (index < mWeight) return mLeft->operator[](index);
        return mRight->operator[](index - mWeight);
    }

    std::string Concatenation::substr(std::size_t start,
                                      std::size_t length) const {
        return subnstr(start, length).to_string();
    }

    nstring Concatenation::subnstr(std::size_t start,
                                   std::size_t length) const {
        if (start >= mWeight) {
            return mRight ? mRight->subnstr(start - mWeight, length) : "";
        }

        nstring ans = mLeft ? mLeft->subnstr(start, length) : "";

        if (start + length > mWeight) {
            ans += mRight ? mRight->subnstr(0, length - (mWeight - start)) : "";
        }
        return ans;
    }

    std::string Concatenation::to_string() const {
        std::string ans = mLeft ? mLeft->to_string() : "";
        ans += mRight ? mRight->to_string() : "";
        return ans;
    }

    nstring Concatenation::to_nstring() const {
        nstring ans = mLeft ? mLeft->to_nstring() : "";
        ans += mRight ? mRight->to_nstring() : "";
        return ans;
    }

    std::pair< Node::Ptr, Node::Ptr > Concatenation::split(
        std::size_t index) const {
        if (index == mWeight) {
            return std::make_pair(mLeft, mRight);
        }

        if (index < mWeight) {
            auto [leftSplit, rightSplit] =
                mLeft ? mLeft->split(index) : std::make_pair(nullptr, nullptr);
            return std::make_pair(leftSplit, std::make_shared< Concatenation >(
                                                 rightSplit, mRight));
        }
        auto [leftSplit, rightSplit] = mRight
                                           ? mRight->split(index - mWeight)
                                           : std::make_pair(nullptr, nullptr);
        return std::make_pair(
            std::make_shared< Concatenation >(mLeft, leftSplit), rightSplit);
    }

    std::vector< Node::Ptr > Concatenation::leaves() const {
        std::vector< Node::Ptr > ans;
        if (mLeft) {
            auto leftLeaves = mLeft->leaves();
            ans.insert(ans.end(), leftLeaves.begin(), leftLeaves.end());
        }
        if (mRight) {
            auto rightLeaves = mRight->leaves();
            ans.insert(ans.end(), rightLeaves.begin(), rightLeaves.end());
        }
        return ans;
    }

    std::pair< std::size_t, std::size_t > Concatenation::pos_from_index(
        std::size_t index) const {
        index = std::min(index, mLength);

        if (index < mWeight) {
            return mLeft->pos_from_index(index);
        }
        auto [line_idx, line_pos] = mRight->pos_from_index(index - mWeight);

        if (line_idx == 0) {
            line_pos +=
                mWeight -
                (mLeft->line_count()
                     ? mLeft->find_line_feed(mLeft->line_count() - 1) + 1
                     : 0);
        }

        return std::make_pair(line_idx + mLineWeight, line_pos);
    }
    std::size_t Concatenation::find_line_feed(std::size_t index) const {
        // if (index >= mLineCount) throw std::out_of_range("Index out of
        // range");
        if (index < mLineWeight) {
            return mLeft->find_line_feed(index);
        }
        return mRight->find_line_feed(index - mLineWeight) + mWeight;
    }

    std::size_t Concatenation::find_word_start(std::size_t index) const {
        if (index >= mWordCount) throw std::out_of_range("Index out of range");
        if (index < mWordWeight) {
            return mLeft->find_word_start(index);
        }
        return mRight->find_word_start(index - mWordWeight) + mWeight;
    }

    std::size_t Concatenation::find_word_at(std::size_t index) const {
        if (index >= mLength) throw std::out_of_range("Index out of range");

        if (index == 0) return 0;

        if (index == mLength - 1) {
            if (operator[](index) != ' ' && operator[](index) != '\n' &&
                                            operator[](index) != '\t')
                return 1;
            return 0;
        }

        if (index < mWeight) {
            return mLeft->find_word_at(index);
        }
        std::size_t word_index =
            mRight->find_word_at(index - mWeight) + mWordWeight;
        return word_index;
    }

    std::size_t Concatenation::line_count() const { return mLineCount; }

    std::size_t Concatenation::word_count() const { return mWordCount; }

}  // namespace rope