#include "dictionary/trie.hpp"

namespace dictionary {
    Trie::Trie() {}

    Trie::~Trie() { delete mRoot; }

    void Trie::insert(const nstring& word) {
        Word* current = mRoot;
        for (std::size_t i = 0; i < word.length(); i++) {
            int c = word[i].codepoint();
            if (current->child(c) == nullptr) {
                current->child(c) = new Word;
                current->child(c)->set_parent(current);
            }
            current = current->child(c);
        }
        current->is_word(true);
    }

    bool Trie::search(const nstring& word) const {
        Word* current = mRoot;
        for (std::size_t i = 0; i < word.length(); i++) {
            int c = word[i].codepoint();
            if (current->child(c) == nullptr) {
                return false;
            }
            current = current->child(c);
        }
        return current->is_word();
    }

    void Trie::get_all_words(std::vector< nstring >& words) const {
        get_all_words(mRoot, words);
    }

    void Trie::get_all_words(Word* root, std::vector< nstring >& words,
                             nstring curWord) const {
        if (root->is_word()) {
            words.push_back(curWord);
        }

        for (int i = 0; i < constants::dictionary::char_limit; i++) {
            if (root->child(i) != nullptr) {
                get_all_words(root->child(i), words, curWord + nchar(i));
            }
        }
    }

};  // namespace dictionary