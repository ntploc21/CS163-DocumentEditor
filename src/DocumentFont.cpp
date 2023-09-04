#include "DocumentFont.hpp"

DocumentFont::DocumentFont() : mFontFactory(nullptr) {}

void DocumentFont::set_font_factory(FontFactory* fonts) {
    mFontFactory = fonts;
}

Font& DocumentFont::get_font(std::size_t id) {
    return mFontFactory->Get(mFonts[id].id);
}

const Font& DocumentFont::get_font(std::size_t id) const {
    return mFontFactory->Get(mFonts.at(id).id);
}

Font& DocumentFont::get_bold_font(std::size_t id) {
    return mFontFactory->Get(mFonts[id].bold_id);
}

const Font& DocumentFont::get_bold_font(std::size_t id) const {
    return mFontFactory->Get(mFonts.at(id).bold_id);
}

Font& DocumentFont::get_italic_font(std::size_t id) {
    return mFontFactory->Get(mFonts[id].italic_id);
}

const Font& DocumentFont::get_italic_font(std::size_t id) const {
    return mFontFactory->Get(mFonts.at(id).italic_id);
}

Font& DocumentFont::get_bold_italic_font(std::size_t id) {
    return mFontFactory->Get(mFonts[id].bold_italic_id);
}

const Font& DocumentFont::get_bold_italic_font(std::size_t id) const {
    return mFontFactory->Get(mFonts.at(id).bold_italic_id);
}

std::size_t DocumentFont::registerFont(FontInfo info) {
    std::size_t id = mFonts.size();
    mFonts[id] = info;
    return id;
}

bool DocumentFont::hasFont(std::string id) const {
    for (auto& font : mFonts) {
        if (font.second.id == id) return true;
    }
    return false;
}

std::vector< std::string > DocumentFont::getFontList() const {
    std::vector< std::string > names;
    for (auto& font : mFonts) {
        names.push_back(font.second.id);
    }
    return names;
}

void DocumentFont::clear() { mFonts.clear(); }
