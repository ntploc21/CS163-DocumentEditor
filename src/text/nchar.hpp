#ifndef TEXT_nchar_HPP
#define TEXT_nchar_HPP

#include <iostream>

#include "constants.hpp"

// reimplement the char class in C++
class nchar {
public:
    enum Type {
        Bold = 0,
        Italic = 1,
        Underline = 2,
        Strikethrough = 3,
        Subscript = 4,
        Superscript = 5,
        NumType = 6
    };

    nchar();
    nchar(const nchar& other);
    nchar(const char* c);
    nchar(int codepoint);

    nchar& operator=(const nchar& other);
    nchar& operator=(const char* c);
    bool operator==(const nchar& other) const;
    bool operator!=(const nchar& other) const;

    int codepoint() const;
    const char* getChar() const;

    friend std::ostream& operator<<(std::ostream& os, const nchar& nchar);

    nchar& toggleBold();
    nchar& toggleItalic();
    nchar& toggleUnderline();
    nchar& toggleStrikethrough();
    nchar& toggleSubscript();
    nchar& toggleSuperscript();

    nchar& toggleType(nchar::Type type);

    bool getType() const;

    void setFontSize(int size);
    int getFontSize() const;

private:
    int mCodepoint{};
    int mType{};

    int mFontSize{constants::document::default_font_size};
};

#endif  // TEXT_nchar_HPP