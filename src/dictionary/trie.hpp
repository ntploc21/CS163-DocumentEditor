#ifndef DICTIONARY_TRIE_HPP
#define DICTIONARY_TRIE_HPP

#include "dictionary/word.hpp"

namespace dictionary {
    class Trie {
    public:
        Trie();
        ~Trie();

        void insert(const nstring& word);
        bool search(const nstring& word) const;
 
        void get_all_words(std::vector< nstring >& words) const;

    private:
        Word* mRoot{new Word};

        void get_all_words(Word* root, std::vector< nstring >& words,
                           nstring curWord = "") const;
=======
    private:
        Word* mRoot{new Word};
    };

};  // namespace dictionary

#endif  // DICTIONARY_TRIE_HPP