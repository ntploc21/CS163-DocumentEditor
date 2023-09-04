#ifndef SEARCH_SEARCH_CPP
#define SEARCH_SEARCH_CPP

#include "search/search.hpp"

#include <algorithm>

void Search::set_pattern(const Rope& pattern) { mPattern = pattern; }

void Search::set_replacement(const Rope& replacement) {
    mReplacement = replacement;
}

// Z-function is used to find all occurrences of a pattern in a string in linear
// time https://cp-algorithms.com/string/z-function.html
void Search::find_in_content(const Rope& text) {
    mMatches.clear();
    mMatchIdx.clear();

    if (mPattern.length() == 0) return;

    Rope s = mPattern.append(Rope(nchar('\0'))).append(text);
    std::size_t n = s.length();
    std::vector< std::size_t > z(n, 0);
    std::size_t l = 0, r = 0;

    for (std::size_t i = 1; i < n; i++) {
        if (i <= r) z[i] = std::min(r - i, z[i - l]);

        while (i + z[i] < n && s[i + z[i]] == s[z[i]]) z[i]++;

        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }

    Cursor cursor{0, -1};
    for (std::size_t i = mPattern.length(); i < n; i++) {
        if (z[i] == mPattern.length()) {
            mMatches.push_back(Cursor{cursor.line, cursor.column});
            mMatchIdx.push_back(i - mPattern.length() - 1);
            // i += mPattern.length() - 1;  // avoid overlapping matches
            // cursor.column += mPattern.length() - 1;
        }
        nchar c = s[i];
        if (c == '\n') {
            cursor.column = 0;
            cursor.line++;
        } else {
            cursor.column++;
        }
    }
}

Rope Search::replace_in_content(const Rope& text) {
    find_in_content(text);

    Rope result = text;
    std::for_each(
        mMatchIdx.rbegin(), mMatchIdx.rend(), [&](const std::size_t pos) {
            result =
                result.erase(pos, mPattern.length()).insert(pos, mReplacement);
        });

    return result;
}

Cursor Search::next_match(Cursor current) const {
    if (mMatches.empty()) return current;

    auto it = std::upper_bound(mMatches.begin(), mMatches.end(), current);

    if (it == mMatches.end()) return mMatches.front();

    return *it;
}

Cursor Search::prev_match(Cursor current) const {
    if (mMatches.empty()) return current;

    auto it = std::lower_bound(mMatches.begin(), mMatches.end(), current);

    if (it == mMatches.begin()) return mMatches.back();
    it--;

    return *it;
}

const std::vector< Cursor >& Search::matches() const { return mMatches; }

const std::vector< std::size_t >& Search::match_idx() const {
    return mMatchIdx;
}

void Search::reset() {
    mMatches.clear();
    mMatchIdx.clear();
}

Rope& Search::pattern() { return mPattern; }

const Rope& Search::pattern() const { return mPattern; }

Rope& Search::replacement() { return mReplacement; }

const Rope& Search::replacement() const { return mReplacement; }

#endif  // SEARCH_SEARCH_CPP