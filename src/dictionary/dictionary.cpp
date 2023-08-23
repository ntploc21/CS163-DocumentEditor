#include "dictionary/dictionary.hpp"

Dictionary::Dictionary() {
    for (auto& root : mRoots) {
        root = new Database;
    }
}

Dictionary::~Dictionary() {
    for (auto& root : mRoots) {
        delete root;
    }
}

bool Dictionary::search(const nstring& word) const {
    return mRoots[static_cast< std::size_t >(mLanguage)]->search(word);
}

void Dictionary::set_language(locale_language language) {
    mLanguage = language;
}

Dictionary::locale_language Dictionary::language() const { return mLanguage; }
