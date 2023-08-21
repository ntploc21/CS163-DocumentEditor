#ifndef TEXT_NCHAR_HPP
#define TEXT_NCHAR_HPP

#include <iostream>

namespace nchar {
    enum Type {
        Bold = 0,
        Italic = 1,
        Underline = 2,
        Strikethrough = 3,
        Subscript = 4,
        Superscript = 5,
        NumType = 6
    };
}

// reimplement the char class in C++
class NChar {
public:
    NChar();
    NChar(const NChar& other);
    NChar(const char* c);
    NChar(int codepoint);

    NChar& operator=(const NChar& other);
    NChar& operator=(const char* c);
    bool operator==(const NChar& other) const;
    bool operator!=(const NChar& other) const;

    int codepoint() const;
    const char* getChar() const;

    friend std::ostream& operator<<(std::ostream& os, const NChar& nchar);

    NChar& toggleBold();
    NChar& toggleItalic();
    NChar& toggleUnderline();
    NChar& toggleStrikethrough();
    NChar& toggleSubscript();
    NChar& toggleSuperscript();

    NChar& toggleType(nchar::Type type);

    bool getType() const;

private:
    int mCodepoint{};
    int mType{};
};

#endif  // TEXT_NCHAR_HPP