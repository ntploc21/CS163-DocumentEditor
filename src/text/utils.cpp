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

#undef MASK