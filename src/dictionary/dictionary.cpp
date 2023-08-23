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

    std::vector< nstring > words;
    while (!file.eof()) {
        std::string word;
        std::getline(file, word);
        if (word.empty()) continue;
        word = tolower(word);

        words.push_back(word);

        mRoots[static_cast< std::size_t >(mLanguage)]->insert(word);
    }
    file.close();

    mSuggester.set_suggestion_keywords(words);
}

// currently my dictionary only support English language so this function still
// use std::string
bool Dictionary::search(const nstring& word) const {
    std::string wordLower = tolower(word.to_string());
    return mRoots[static_cast< std::size_t >(mLanguage)]->search(wordLower);
}

// currently my dictionary only support English language so this function still
// use std::string
std::vector< nstring > Dictionary::suggest(const nstring& word) {
    std::string wordL = tolower(word.to_string());
    mSuggester.set_pattern(wordL);
    return mSuggester.suggest();
}

void Dictionary::set_language(locale_language language) {
    mLanguage = language;
}

Dictionary::locale_language Dictionary::language() const { return mLanguage; }
