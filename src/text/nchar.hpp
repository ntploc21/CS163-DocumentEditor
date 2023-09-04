#ifndef TEXT_nchar_HPP
#define TEXT_nchar_HPP

#include <iostream>

#include "constants.hpp"
#include "raylib.h"

// reimplement the char class in C++
class nchar {
public:
    const Color default_text_color = BLACK;

    const Color default_background_color = Color{0, 0, 0, 0};

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

    int getType() const;
    void setType(int type);

    void setFontSize(int size);
    int getFontSize() const;

    void setFontId(std::size_t id);
    std::size_t getFontId() const;

    void setColor(Color color);
    Color getColor() const;

    void setBackgroundColor(Color color);
    Color getBackgroundColor() const;

    void setLink(std::string link);
    std::string getLink() const;
    bool hasLink() const;

    bool isBold() const;
    bool isItalic() const;
    bool isUnderline() const;
    bool isStrikethrough() const;
    bool isSubscript() const;
    bool isSuperscript() const;

private:
    int mCodepoint{};
    int mType{};

    int mFontSize{constants::document::default_font_size};

    Color mColor{constants::document::default_text_color};

    Color mBackgroundColor{constants::document::default_background_color};

    std::size_t mFontId{0};

    std::string mLink{};
};

#endif  // TEXT_nchar_HPP