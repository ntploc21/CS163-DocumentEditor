#include "text/nstring.hpp"

#include <cstring>

#include "text/utils.hpp"

NString::NString() {}

NString::NString(const NString& other)
    : mChars(other.mChars), mLength(other.mLength) {}

NString::NString(const std::string& str) { *this = str; }

NString& NString::operator=(const NString& other) {
    mChars = other.mChars;
    mLength = other.mLength;
    return *this;
}

NString& NString::operator=(const std::string& str) {
    mChars.clear();

    for (int i = 0; i < str.length();) {
        if ((str[i] & 0x80) == 0) {
            mChars.push_back(NChar(str[i]));
            i += 1;
        } else if ((str[i] & 0xE0) == 0xC0) {
            mChars.push_back(NChar(str.substr(i, 2).c_str()));
            i += 2;
        } else if ((str[i] & 0xF0) == 0xE0) {
            mChars.push_back(NChar(str.substr(i, 3).c_str()));
            i += 3;
        } else if ((str[i] & 0xF8) == 0xF0) {
            mChars.push_back(NChar(str.substr(i, 4).c_str()));
            i += 4;
        }
    }
    mLength = mChars.size();

    return *this;
}

bool NString::operator==(const NString& other) const {
    if (length() != other.length()) return false;

    for (int i = 0; i < mChars.size(); ++i) {
        if (mChars[i] != other.mChars[i]) return false;
    }

    return true;
}

bool NString::operator!=(const NString& other) const {
    return !(*this == other);
}

NString& NString::operator+=(const NString& other) {
    for (int i = 0; i < other.length(); ++i) {
        mChars.push_back(other.mChars[i]);
    }

    return *this;
}

NString NString::operator+(const NString& other) const {
    NString result(*this);
    result += other;
    return result;
}

NString& NString::operator+=(const NChar& other) {
    mChars.push_back(other);
    mLength++;
    return *this;
}

NString NString::operator+(const NChar& other) const {
    NString result(*this);
    result += other;
    return result;
}

NChar& NString::operator[](int index) {
    if (index >= length()) throw std::out_of_range("Index out of range");
    return mChars[index];
}

const NChar& NString::operator[](int index) const {
    if (index >= length()) throw std::out_of_range("Index out of range");
    return mChars[index];
}

std::size_t NString::length() const { return mChars.size(); }

std::string NString::to_string() const {
    std::string result;

    for (int i = 0; i < length(); ++i) {
        result += mChars[i].getChar();
    }

    return result;
}

const char* NString::c_str() const {
    std::string result = to_string();

    char* c = new char[result.length() + 1];
    std::strcpy(c, result.c_str());

    return c;
}

NString NString::substr(std::size_t start, std::size_t length) const {
    if (start >= mLength) {
        return NString("");
    }

    length = std::min(length, mLength - start);

    NString result;
    for (int i = start; i < start + length; ++i) {
        result += mChars[i];
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const NString& nstring) {
    os << nstring.to_string();
    return os;
}

#define MASK(i) (1LL << (i))

NString& NString::toggleBold(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Bold);
}

NString& NString::toggleItalic(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Italic);
}

NString& NString::toggleUnderline(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Underline);
}

NString& NString::toggleStrikethrough(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Strikethrough);
}

NString& NString::toggleSubscript(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Subscript);
}

NString& NString::toggleSuperscript(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Superscript);
}

NString& NString::toggleType(std::size_t start, std::size_t length,
                             nchar::Type type) {
    if (start >= mLength) return *this;
    length = std::min(length, mLength - start);

    bool typeOn = !isSameType(*this, type, start, length);
    for (int i = start; i < start + length; ++i) {
        if (mChars[i].getType() ^ MASK(type)) {
            switch (type) {
                case nchar::Bold:
                    mChars[i].toggleBold();
                    break;
                case nchar::Italic:
                    mChars[i].toggleItalic();
                    break;
                case nchar::Underline:
                    mChars[i].toggleUnderline();
                    break;
                case nchar::Strikethrough:
                    mChars[i].toggleStrikethrough();
                    break;
                case nchar::Subscript:
                    mChars[i].toggleSubscript();
                    break;
                case nchar::Superscript:
                    mChars[i].toggleSuperscript();
                    break;
                default:
                    break;
            }
        }
    }
    return *this;
}