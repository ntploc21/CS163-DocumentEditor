#ifndef DICTIONARY_DICTIONARY_HPP
#define DICTIONARY_DICTIONARY_HPP

#include "autocomplete/suggester.hpp"
#include "constants.hpp"
#include "dictionary/trie.hpp"

class Dictionary {
public:
    using Database = dictionary::Trie;
    using locale_language = constants::dictionary::locale_language;

    Dictionary();
    ~Dictionary();

    void loadDatabase(const std::string& path);

    bool search(const nstring& word) const;

    std::vector< nstring > suggest(const nstring& word);

    void set_language(locale_language language);
    locale_language language() const;

private:
    Suggester mSuggester{};

    std::array< Database*, locale_language::NUM_LANGUAGES > mRoots{};
    locale_language mLanguage{constants::dictionary::default_language};
};

#endif  // DICTIONARY_DICTIONARY_HPP