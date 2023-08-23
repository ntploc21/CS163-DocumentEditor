#include "dictionary/word.hpp"

namespace dictionary {

    Word::~Word() {
        for (auto& child : mChildren) {
            delete child;
        }
    }

    void Word::set_parent(Word* parent) { mParent = parent; }

    Word*& Word::parent() { return mParent; }

    std::array< Word*, constants::dictionary::char_limit >& Word::children() {
        return mChildren;
    }

    const std::array< Word*, constants::dictionary::char_limit >&
    Word::children() const {
        return mChildren;
    }

    Word*& Word::child(char c) {
        return mChildren[static_cast< std::size_t >(c)];
    }

    void Word::is_word(bool is_word) { mIsWord = is_word; }

    bool Word::is_word() const { return mIsWord; }

};  // namespace dictionary