#ifndef SEARCH_SEARCH_HPP
#define SEARCH_SEARCH_HPP

#include "cursor.hpp"
#include "rope/rope.hpp"

class Search {
public:
    void set_pattern(const Rope& pattern);
    void set_replacement(const Rope& replacement);

    void find_in_content(const Rope& text);
    [[no_discard]] Rope replace_in_content(const Rope& text);

    Cursor next_match(Cursor current) const;
    Cursor prev_match(Cursor current) const;
    const std::vector< Cursor >& matches() const;
    const std::vector< std::size_t >& match_idx() const;

    void reset();

    Rope& pattern();
    const Rope& pattern() const;

    Rope& replacement();
    const Rope& replacement() const;

private:
    Rope mPattern{};
    Rope mReplacement{};
    std::vector< Cursor > mMatches{};
    std::vector< std::size_t > mMatchIdx{};
};

#endif  // SEARCH_SEARCH_HPP