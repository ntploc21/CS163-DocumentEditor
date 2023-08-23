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

};  // namespace dictionary