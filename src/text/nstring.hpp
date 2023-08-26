#ifndef TEXT_nstring_HPP
#define TEXT_nstring_HPP

#include <vector>

#include "text/nchar.hpp"

class nstring {
public:
    nstring();
    nstring(const nchar& other);
    nstring(const nstring& other);
    nstring(const std::string& str);
    nstring(const char* str);

    nstring& operator=(const char* str);
    nstring& operator=(const nchar& other);
    nstring& operator=(const nstring& other);
    nstring& operator=(const std::string& str);
    bool operator==(const nstring& other) const;
    bool operator!=(const nstring& other) const;

    nstring& operator+=(const nstring& other);
    nstring operator+(const nstring& other) const;

    nstring& operator+=(const nchar& other);
    nstring operator+(const nchar& other) const;

    nchar& operator[](int index);
    const nchar& operator[](int index) const;

    std::size_t length() const;
    std::string to_string() const;
    const char* c_str() const;
    nstring substr(std::size_t start, std::size_t length) const;
    nstring substr(std::size_t start) const;

    friend std::ostream& operator<<(std::ostream& os, const nstring& nstring);

    nstring& toggleBold(std::size_t start, std::size_t length);
    nstring& toggleItalic(std::size_t start, std::size_t length);
    nstring& toggleUnderline(std::size_t start, std::size_t length);
    nstring& toggleStrikethrough(std::size_t start, std::size_t length);
    nstring& toggleSubscript(std::size_t start, std::size_t length);
    nstring& toggleSuperscript(std::size_t start, std::size_t length);

    void setFontSize(int size);
    int getFontSize() const;

    void setFontId(std::size_t id);
    int getFontId() const;

private:
    nstring& toggleType(std::size_t start, std::size_t length,
                        nchar::Type type);

    std::vector< nchar > mChars{};
    std::size_t mLength{};

    int mFontSize{constants::document::default_font_size};

    int mFontId{0};
};

#endif  // TEXT_nstring_HPP