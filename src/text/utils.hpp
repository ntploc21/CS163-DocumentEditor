#ifndef TEXT_UTILS_HPP
#define TEXT_UTILS_HPP

#include "text/nstring.hpp"

bool isSameType(const nstring& str, nchar::Type type, std::size_t start,
                std::size_t length);

bool cmpColor(const Color& a, const Color& b);

nstring tolower(nstring str);

nstring toupper(nstring str);

std::string tolower(std::string str);

std::string toupper(std::string str);

#endif  // TEXT_UTILS_HPP