#include "text/nchar.hpp"

nchar::nchar() {}

nchar::nchar(const nchar& other)
    : mCodepoint{other.mCodepoint}, mType{other.mType} {}

nchar::nchar(const char* c) { *this = c; }

nchar::nchar(int codepoint) : mCodepoint{codepoint} {}

nchar& nchar::operator=(const nchar& other) {
    mCodepoint = other.mCodepoint;
    mType = other.mType;
    return *this;
}

nchar& nchar::operator=(const char* c) {
    int unicode = 0;
    int i = 0;
    int len = 0;

    while (c[i] != '\0') {
        if ((c[i] & 0x80) == 0) {
            unicode = c[i];
            len = 1;
        } else if ((c[i] & 0xE0) == 0xC0) {
            unicode = c[i] & 0x1F;
            len = 2;
        } else if ((c[i] & 0xF0) == 0xE0) {
            unicode = c[i] & 0x0F;
            len = 3;
        } else if ((c[i] & 0xF8) == 0xF0) {
            unicode = c[i] & 0x07;
            len = 4;
        } else {
            unicode = 0;
            len = 0;
        }

        for (int j = 1; j < len; ++j) {
            unicode = (unicode << 6) | (c[i + j] & 0x3F);
        }

        i += len;
    }

    mCodepoint = unicode;
    return *this;
}

bool nchar::operator==(const nchar& other) const {
    return mCodepoint == other.mCodepoint && mType == other.mType;
}

bool nchar::operator!=(const nchar& other) const { return !(*this == other); }

int nchar::codepoint() const { return mCodepoint; }

const char* nchar::getChar() const {
    char* c;
    int unicode = mCodepoint;

    if (unicode < 128) {
        c = new char[2];
        c[0] = unicode;
        c[1] = '\0';
    } else if (unicode < 2048) {
        c = new char[3];
        c[0] = 192 | (unicode >> 6);
        c[1] = 128 | (unicode & 63);
        c[2] = '\0';
    } else if (unicode < 65536) {
        c = new char[4];
        c[0] = 224 | (unicode >> 12);
        c[1] = 128 | ((unicode >> 6) & 63);
        c[2] = 128 | (unicode & 63);
        c[3] = '\0';
    } else {
        c = new char[5];
        c[0] = 240 | (unicode >> 18);
        c[1] = 128 | ((unicode >> 12) & 63);
        c[2] = 128 | ((unicode >> 6) & 63);
        c[3] = 128 | (unicode & 63);
        c[4] = '\0';
    }

    return c;
}

std::ostream& operator<<(std::ostream& os, const nchar& nchar) {
    os << nchar.getChar();
    return os;
}

#define MASK(i) (1LL << (i))

nchar& nchar::toggleBold() {
    mType ^= MASK(Bold);
    return *this;
}

nchar& nchar::toggleItalic() {
    mType ^= MASK(Italic);
    return *this;
}

nchar& nchar::toggleUnderline() {
    mType ^= MASK(Underline);
    return *this;
}

nchar& nchar::toggleStrikethrough() {
    mType ^= MASK(Strikethrough);
    return *this;
}

nchar& nchar::toggleSubscript() {
    mType ^= MASK(Subscript);
    return *this;
}

nchar& nchar::toggleSuperscript() {
    mType ^= MASK(Superscript);
    return *this;
}

nchar& nchar::toggleType(Type type) {
    mType ^= MASK(type);
    return *this;
}

bool nchar::getType() const { return mType; }

#undef MASK
