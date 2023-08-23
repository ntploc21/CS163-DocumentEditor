#include "text/nstring.hpp"

#include <cstring>

#include "text/utils.hpp"

nstring::nstring() {}

nstring::nstring(const nchar& other) { *this = other; }

nstring::nstring(const nstring& other)
    : mChars{other.mChars}, mLength{other.mLength} {}

nstring::nstring(const std::string& str) { *this = str; }

nstring::nstring(const char* str) { *this = str; }

nstring& nstring::operator=(const char* str) {
    std::string s = str;
    *this = s;
    return *this;
}

nstring& nstring::operator=(const nchar& other) {
    mChars.clear();
    mChars.push_back(other);
    mLength = 1;
    return *this;
}

nstring& nstring::operator=(const nstring& other) {
    mChars = other.mChars;
    mLength = other.mLength;
    return *this;
}

nstring& nstring::operator=(const std::string& str) {
    mChars.clear();

    for (int i = 0; i < str.length();) {
        if ((str[i] & 0x80) == 0) {
            mChars.push_back(nchar(str[i]));
            i += 1;
        } else if ((str[i] & 0xE0) == 0xC0) {
            mChars.push_back(nchar(str.substr(i, 2).c_str()));
            i += 2;
        } else if ((str[i] & 0xF0) == 0xE0) {
            mChars.push_back(nchar(str.substr(i, 3).c_str()));
            i += 3;
        } else if ((str[i] & 0xF8) == 0xF0) {
            mChars.push_back(nchar(str.substr(i, 4).c_str()));
            i += 4;
        }
    }
    mLength = mChars.size();

    return *this;
}

bool nstring::operator==(const nstring& other) const {
    if (length() != other.length()) return false;

    for (int i = 0; i < mChars.size(); ++i) {
        if (mChars[i] != other.mChars[i]) return false;
    }

    return true;
}

bool nstring::operator!=(const nstring& other) const {
    return !(*this == other);
}

nstring& nstring::operator+=(const nstring& other) {
    for (int i = 0; i < other.length(); ++i) {
        mChars.push_back(other.mChars[i]);
    }

    return *this;
}

nstring nstring::operator+(const nstring& other) const {
    nstring result(*this);
    result += other;
    return result;
}

nstring& nstring::operator+=(const nchar& other) {
    mChars.push_back(other);
    mLength++;
    return *this;
}

nstring nstring::operator+(const nchar& other) const {
    nstring result(*this);
    result += other;
    return result;
}

nchar& nstring::operator[](int index) {
    if (index >= length()) throw std::out_of_range("Index out of range");
    return mChars[index];
}

const nchar& nstring::operator[](int index) const {
    if (index >= length()) throw std::out_of_range("Index out of range");
    return mChars[index];
}

std::size_t nstring::length() const { return mChars.size(); }

std::string nstring::to_string() const {
    std::string result;

    for (int i = 0; i < length(); ++i) {
        result += mChars[i].getChar();
    }

    return result;
}

const char* nstring::c_str() const {
    std::string result = to_string();

    char* c = new char[result.length() + 1];
    std::strcpy(c, result.c_str());

    return c;
}

nstring nstring::substr(std::size_t start, std::size_t length) const {
    if (start >= mLength) {
        return nstring("");
    }

    length = std::min(length, mLength - start);

    nstring result;
    for (int i = start; i < start + length; ++i) {
        result += mChars[i];
    }
    return result;
}

nstring nstring::substr(std::size_t start) const {
    if (start >= mLength) {
        return nstring("");
    }

    nstring result;
    for (int i = start; i < mLength; ++i) {
        result += mChars[i];
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const nstring& nstring) {
    os << nstring.to_string();
    return os;
}

#define MASK(i) (1LL << (i))

nstring& nstring::toggleBold(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Bold);
}

nstring& nstring::toggleItalic(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Italic);
}

nstring& nstring::toggleUnderline(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Underline);
}

nstring& nstring::toggleStrikethrough(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Strikethrough);
}

nstring& nstring::toggleSubscript(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Subscript);
}

nstring& nstring::toggleSuperscript(std::size_t start, std::size_t length) {
    return toggleType(start, length, nchar::Superscript);
}

nstring& nstring::toggleType(std::size_t start, std::size_t length,
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