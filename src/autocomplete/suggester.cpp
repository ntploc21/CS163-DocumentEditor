#include "autocomplete/suggester.hpp"

#include <algorithm>

void Suggester::set_suggestion_keywords(
    const std::vector< nstring >& keywords) {
    mSuggestionKeywords = keywords;
}

void Suggester::set_pattern(const nstring& pattern) {
    mPattern = pattern;
    mMatches.clear();
    for (std::size_t i = 0; i < mSuggestionKeywords.size(); i++) {
        int score = calculate_score(mSuggestionKeywords[i], mPattern);
        if (score > 0) {
            mMatches.push_back({i, score});
        }
    }
    std::sort(mMatches.begin(), mMatches.end(),
              [](const ScoredMatch& a, const ScoredMatch& b) {
                  return a.score > b.score;
              });
}

std::vector< nstring > Suggester::suggest() const {
    std::vector< nstring > suggestions;
    for (const auto& match : mMatches) {
        suggestions.push_back(mSuggestionKeywords[match.keyword_idx]);
    }
    return suggestions;
}

int Suggester::calculate_score(const nstring& keyword, const nstring& pattern) {
    // Needleman–Wunsch algorithm
    // https://en.wikipedia.org/wiki/Needleman–Wunsch_algorithm

    constexpr int match = 8;
    constexpr int mismatch = -1;
    constexpr int indel = -2;

    int max_score = 0;

    std::vector< std::vector< int > > matrix(
        pattern.length() + 1, std::vector< int >(keyword.length() + 1, 0));

    for (std::size_t i = 1; i <= pattern.length(); i++) {
        matrix[i][0] = indel * i;
    }

    for (std::size_t j = 1; j <= keyword.length(); j++) {
        matrix[0][j] = indel * j;
    }

    for (std::size_t i = 1; i <= pattern.length(); i++) {
        for (std::size_t j = 1; j <= keyword.length(); j++) {
            int match_score =
                matrix[i - 1][j - 1] +
                (pattern[i - 1] == keyword[j - 1] ? match : mismatch);

            int del_score = matrix[i - 1][j] + indel;
            int ins_score = matrix[i][j - 1] + indel;

            matrix[i][j] = std::max({match_score, del_score, ins_score});

            max_score = std::max(max_score, matrix[i][j]);
        }
    }

    return max_score;
}