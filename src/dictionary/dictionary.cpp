#include "dictionary/dictionary.hpp"

#include <fstream>

#include "text/utils.hpp"

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

void Dictionary::loadDatabase(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not load database from file: " + path);
    }

    while (!file.eof()) {
        std::string word;
        std::getline(file, word);
        if (word.empty()) continue;
        word = tolower(word);

        mRoots[static_cast< std::size_t >(mLanguage)]->insert(word);
    }

    file.close();
}

// currently my dictionary only support English language so this function still
// use std::string
bool Dictionary::search(const nstring& word) const {
    std::string wordLower = tolower(word.to_string());
    return mRoots[static_cast< std::size_t >(mLanguage)]->search(wordLower);
}

void Dictionary::set_language(locale_language language) {
    mLanguage = language;
}

Dictionary::locale_language Dictionary::language() const { return mLanguage; }
