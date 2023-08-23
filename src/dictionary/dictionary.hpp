#ifndef DICTIONARY_DICTIONARY_HPP
#define DICTIONARY_DICTIONARY_HPP

#include "constants.hpp"
#include "dictionary/trie.hpp"

class Dictionary {
public:
    using Database = dictionary::Trie;
    using locale_language = constants::dictionary::locale_language;

    Dictionary();
    ~Dictionary();

    bool search(const nstring& word) const;

    void set_language(locale_language language);
    locale_language language() const;

private:
    std::array< Database*, locale_language::NUM_LANGUAGES > mRoots{};
    locale_language mLanguage{constants::dictionary::default_language};
};

#endif  // DICTIONARY_DICTIONARY_HPP