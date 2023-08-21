#ifndef TEXT_UTILS_HPP
#define TEXT_UTILS_HPP

#include "text/nstring.hpp"

bool isSameType(const NString& str, nchar::Type type, std::size_t start,
                std::size_t length);

#endif  // TEXT_UTILS_HPP