#include "rope/node_concatenation.hpp"

namespace rope {
    Concatenation::Concatenation(const Node::Ptr& left, const Node::Ptr& right)
        : mLeft(std::move(left)), mRight(std::move(right)) {
        mWeight = mLeft ? mLeft->length() : 0;
        mLength = mWeight + (mRight ? mRight->length() : 0);

        std::size_t leftDepth = mLeft ? mLeft->depth() : 0;
        std::size_t rightDepth = mRight ? mRight->depth() : 0;
        mDepth = std::max(leftDepth, rightDepth) + 1;
    }

    Concatenation::Concatenation(const Concatenation& other)
        : Concatenation(other.mLeft, other.mRight) {}

    char Concatenation::operator[](std::size_t index) const {
        if (index < mWeight) return mLeft->operator[](index);
        return mRight->operator[](index - mWeight);
    }

    std::string Concatenation::substr(std::size_t start,
                                      std::size_t length) const {
        if (start >= mWeight) {
            return mRight ? mRight->substr(start - mWeight, length) : "";
        }

        std::string ans = mLeft ? mLeft->substr(start, length) : "";

        if (start + length > mWeight) {
            ans += mRight ? mRight->substr(0, length - (mWeight - start)) : "";
        }
        return ans;
    }

    std::string Concatenation::to_string() const {
        std::string ans = mLeft ? mLeft->to_string() : "";
        ans += mRight ? mRight->to_string() : "";
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

}  // namespace rope