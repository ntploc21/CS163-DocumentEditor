#ifndef DICTIONARY_NODE_HPP
#define DICTIONARY_NODE_HPP

#include <array>

#include "constants.hpp"
#include "text/nstring.hpp"

namespace dictionary {

    class Word {
    public:
        void set_parent(Word* parent);
        Word*& parent();

        std::array< Word*, constants::dictionary::char_limit >& children();
        const std::array< Word*, constants::dictionary::char_limit >& children()
            const;
        Word*& child(char c);

        virtual ~Word();

        void is_word(bool is_word);

        bool is_word() const;

    protected:
        Word* mParent{};
        std::array< Word*, constants::dictionary::char_limit > mChildren{};

        bool mIsWord{};
    };

};  // namespace dictionary

#endif  // DICTIONARY_NODE_HPP