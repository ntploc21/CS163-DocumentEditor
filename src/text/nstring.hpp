#ifndef TEXT_NSTRING_HPP
#define TEXT_NSTRING_HPP

#include <vector>

#include "text/nchar.hpp"

class NString {
public:
    NString();
    NString(const NString& other);
    NString(const std::string& str);

    NString& operator=(const NString& other);
    NString& operator=(const std::string& str);
    bool operator==(const NString& other) const;
    bool operator!=(const NString& other) const;

    NString& operator+=(const NString& other);
    NString operator+(const NString& other) const;

    NString& operator+=(const NChar& other);
    NString operator+(const NChar& other) const;

    NChar& operator[](int index);
    const NChar& operator[](int index) const;

    std::size_t length() const;
    std::string to_string() const;
    const char* c_str() const;
    NString substr(std::size_t start, std::size_t length) const;

    friend std::ostream& operator<<(std::ostream& os, const NString& nstring);

    NString& toggleBold(std::size_t start, std::size_t length);
    NString& toggleItalic(std::size_t start, std::size_t length);
    NString& toggleUnderline(std::size_t start, std::size_t length);
    NString& toggleStrikethrough(std::size_t start, std::size_t length);
    NString& toggleSubscript(std::size_t start, std::size_t length);
    NString& toggleSuperscript(std::size_t start, std::size_t length);

private:
    NString& toggleType(std::size_t start, std::size_t length,
                        nchar::Type type);

    std::vector< NChar > mChars{};
    std::size_t mLength{};
};

#endif  // TEXT_NSTRING_HPP