#ifndef DOCUMENTFONT_HPP
#define DOCUMENTFONT_HPP

#include <map>
#include <string>
#include <vector>

#include "FontFactory.hpp"
#include "raylib.h"

struct FontInfo {
    std::string id;
    std::string bold_id;
    std::string italic_id;
    std::string bold_italic_id;
};

class DocumentFont {
public:
    DocumentFont();

    void set_font_factory(FontFactory* fonts);

    Font& get_font(std::size_t id);
    const Font& get_font(std::size_t id) const;

    Font& get_bold_font(std::size_t id);
    const Font& get_bold_font(std::size_t id) const;

    Font& get_italic_font(std::size_t id);
    const Font& get_italic_font(std::size_t id) const;

    Font& get_bold_italic_font(std::size_t id);
    const Font& get_bold_italic_font(std::size_t id) const;

    std::size_t registerFont(FontInfo info);

    std::vector< std::string > getFontList() const;

    void clear();

private:
    std::map< std::size_t, FontInfo > mFonts;

    FontFactory* mFontFactory;
};

#endif  // DOCUMENTFONT_HPP