#ifndef AUTOCOMPLETE_SUGGESTER_HPP
#define AUTOCOMPLETE_SUGGESTER_HPP

#include <vector>

#include "text/nstring.hpp"

class Suggester {
public:
    void set_pattern(const nstring& pattern);

    void set_suggestion_keywords(const std::vector< nstring >& keywords);

    std::vector< nstring > suggest() const;

private:
    struct ScoredMatch {
        std::size_t keyword_idx{};
        int score{};
    };

    nstring mPattern{};
    std::vector< nstring > mSuggestionKeywords{};
    std::vector< ScoredMatch > mMatches{};

    static int calculate_score(const nstring& keyword, const nstring& pattern);
};

#endif  // AUTOCOMPLETE_SUGGESTER_HPP