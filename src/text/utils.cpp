#include "text/utils.hpp"

#define MASK(i) (1LL << (i))

bool isSameType(const nstring& str, nchar::Type type, std::size_t start,
                std::size_t length) {
    length = std::min(length, str.length() - start);
    for (int i = start; i < start + length; ++i) {
        if (!(str[i].getType() & MASK(type))) return false;
    }
    return true;
}

nstring tolower(nstring str) {
    nstring result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        result += tolower(str[i].codepoint());
    }
    return result;
}

nstring toupper(nstring str) {
    nstring result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        result += toupper(str[i].codepoint());
    }
    return result;
}

std::string tolower(std::string str) {
    std::string result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        result += tolower(str[i]);
    }
    return result;
}

std::string toupper(std::string str) {
    std::string result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        result += toupper(str[i]);
    }
    return result;
}

#undef MASK