#include "document.hpp"

#include <algorithm>
#include <iomanip>

#include "constants.hpp"
#include "utils.hpp"

Document::Document() : mRope{"\n"} {}

Document::Document(std::string filename) : mFilename{filename} {
    // not implemented yet
}

void Document::set_font_factory(FontFactory* fonts) { mFonts = fonts; }

void Document::set_document_fonts(DocumentFont* docFonts) {
    mDocFonts = docFonts;
}

void Document::set_dictionary(Dictionary* dictionary) {
    mDictionary = dictionary;
}

Rope& Document::rope() { return mRope; }

const Rope& Document::rope() const { return mRope; }

Cursor& Document::cursor() { return mCursor; }

const Cursor& Document::cursor() const { return mCursor; }

void Document::set_cursor(Cursor cursor) { mCursor = cursor; }

std::string& Document::filename() { return mFilename; }

const std::string& Document::filename() const { return mFilename; }

Cursor& Document::select_orig() { return mSelectOrig; }

const Cursor& Document::select_orig() const { return mSelectOrig; }

const Cursor& Document::select_start() const {
    return mCursor < mSelectOrig ? mCursor : mSelectOrig;
}

const Cursor& Document::select_end() const {
    return mCursor > mSelectOrig ? mCursor : mSelectOrig;
}

void Document::cursor_move_line(int delta) {
    mCursor.line = std::clamp(mCursor.line + delta, 0,
                              static_cast< int >(mRope.line_count()) - 1);

    cursor_move_column(0);
}

void Document::cursor_move_column(int delta) {
    int lineLength = mRope.line_length(mCursor.line);

    // int right_offset =
    //     (mCursor.line + 1 == static_cast< int >(mRope.line_count()));
    mCursor.column = std::clamp(mCursor.column + delta, 0, lineLength);

    if (mCursor.column < 0) {
        mCursor.column = 0;
    }
}

void Document::cursor_move_next_word() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    if (pos == mRope.length()) return;
    cursor_move_next_char();

    int c = mRope[pos].codepoint();
    bool alnum_word = !!std::isalnum(c);
    bool punct_word = !!std::ispunct(c);

    if (alnum_word) {
        for (; pos + 2 < mRope.length() && std::isalnum(mRope[pos].codepoint());
             ++pos) {
            cursor_move_next_char();
        }
    } else if (pos + 2 < mRope.length() && punct_word) {
        ++pos;
        cursor_move_next_char();
        if (std::ispunct(mRope[pos].codepoint())) {
            return;
        }
    }

    if (std::isspace(c)) {
        for (; pos + 2 < mRope.length() && std::isspace(mRope[pos].codepoint());
             ++pos) {
            cursor_move_next_char();
        }
    }
}

void Document::cursor_move_prev_word() {
    if (mCursor.column == 0 && mCursor.line == 0) return;
    cursor_move_prev_char();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    int c = mRope[pos].codepoint();

    for (; pos > 0 && std::isspace(mRope[pos].codepoint()); --pos) {
        cursor_move_prev_char();
    }

    if (std::ispunct(c)) return;

    for (; pos > 0 && std::isalnum(mRope[pos - 1].codepoint()); --pos) {
        cursor_move_prev_char();
    }
}

void Document::cursor_move_next_char() {
    if (mCursor.column == mRope.line_length(mCursor.line)) {
        if (mCursor.line + 1 < mRope.line_count()) {
            ++mCursor.line;
            mCursor.column = 0;
        }
    } else {
        ++mCursor.column;
    }
}

void Document::cursor_move_prev_char() {
    if (mCursor.column > 0) {
        --mCursor.column;
    } else if (mCursor.line > 0) {
        --mCursor.line;
        mCursor.column = mRope.line_length(mCursor.line);
    }
}

Cursor Document::pos_on_mouse() const {
    Vector2 pos = GetMousePosition();
    pos.x -=
        constants::document::padding_left +
        std::max(
            0,
            (GetScreenWidth() - constants::document::default_view_width) / 2);
    pos.y -= constants::document::padding_top + constants::document::margin_top;

    auto it = std::lower_bound(displayPositions.begin(), displayPositions.end(),
                               pos, utils::cmpVector2);
    if (it == displayPositions.begin()) {
        return {0, 0};
    }

    pos.y = (--it)->y;

    std::size_t document_pos =
        lower_bound(displayPositions.begin(), displayPositions.end(), pos,
                    utils::cmpVector2) -
        displayPositions.begin();
    if (document_pos == displayPositions.size()) {
        document_pos = displayPositions.size() - 1;
    }

    if (displayPositions[document_pos].y != pos.y) {
        document_pos--;
    }

    auto [line, column] = mRope.pos_from_index(document_pos);

    return Cursor{static_cast< int >(line), static_cast< int >(column)};
}

void Document::insert_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }

    processWordWrap();
}

void Document::append_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }

    processWordWrap();
}

void Document::erase_at_cursor() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    if (pos == 0) return;

    cursor_move_prev_char();
    mRope = mRope.erase(pos - 1, 1);

    processWordWrap();
}

void Document::erase_selected() {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    erase_range(start, end);
}

void Document::erase_range(std::size_t start, std::size_t end) {
    save_snapshot();

    mRope = mRope.erase(start, end - start);

    set_cursor(select_start());
    if (mRope.length() == 0) {
        mRope = mRope.append("\n");
    }
    processWordWrap();
}

void Document::copy_selected() {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    copy_range(start, end);
}

void Document::copy_range(std::size_t start, std::size_t end) {
    std::string text = mRope.substr(start, end - start);
    SetClipboardText(text.c_str());
}

void Document::save_snapshot() {
    mUndo.push_back({mRope, mCursor});
    mRedo.clear();
}

void Document::undo() {
    if (mUndo.empty()) return;

    mRedo.push_back({mRope, mCursor});
    const auto& [prev_rope, prev_cursor] = mUndo.back();

    mRope = prev_rope;
    set_cursor(prev_cursor);

    mUndo.pop_back();

    processWordWrap();
}

void Document::redo() {
    if (mRedo.empty()) return;

    mUndo.push_back({mRope, mCursor});
    const auto& [next_rope, next_cursor] = mRedo.back();

    mRope = next_rope;
    set_cursor(next_cursor);

    mRedo.pop_back();

    processWordWrap();
}

Vector2 Document::get_display_positions(std::size_t index) const {
    if (index >= displayPositions.size()) {
        return {0, 0};
    }

    return displayPositions[index];
}

void Document::turn_on_selecting() { mIsSelecting = true; }

void Document::turn_off_selecting() {
    mSelectOrig = {-1, -1};
    mIsSelecting = false;
}

bool Document::is_selecting() const { return mIsSelecting; }

bool Document::check_word_at_cursor() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    nstring word = mRope.subnstr(left, right - left);
    return mDictionary->search(word);
}

std::vector< nstring > Document::suggest_at_cursor() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    nstring word = mRope.subnstr(left, right - left);
    return mDictionary->suggest(word);
}

nstring Document::get_selected_text() const {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    return mRope.subnstr(start, end - start);
}

std::vector< std::pair< std::size_t, nstring > > Document::get_outline() const {
    std::vector< std::pair< std::size_t, nstring > > headings;

    // return some examples
    headings.push_back({1, "Heading 1 hello everyone xin chao cac ban"});
    headings.push_back({2, "Heading 2"});
    headings.push_back({3, "Heading 3"});
    headings.push_back({4, "Heading 4"});
    headings.push_back({5, "Heading 5"});

    return headings;

    // std::size_t pos = 0;
    // while (pos < mRope.length()) {
    //     if (mRope[pos].codepoint() == '#') {
    //         int level = 0;
    //         while (pos < mRope.length() && mRope[pos].codepoint() == '#') {
    //             ++level;
    //             ++pos;
    //         }

    //         while (pos < mRope.length() &&
    //         std::isspace(mRope[pos].codepoint()))
    //             ++pos;

    //         int start = pos;
    //         while (pos < mRope.length() && mRope[pos].codepoint() != '\n')
    //             ++pos;

    //         nstring heading = mRope.subnstr(start, pos - start);

    //         headings.push_back({level, heading});
    //     } else {
    //         ++pos;
    //     }
    // }

    // return headings;
}

void Document::underline_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleUnderline(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);
}

void Document::strikethrough_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleStrikethrough(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);
}

void Document::bold_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleBold(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);

    processWordWrap();
}

void Document::italic_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleItalic(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);

    processWordWrap();
}

void Document::subscript_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleSubscript(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);

    processWordWrap();
}

void Document::superscript_selected() {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.toggleSuperscript(0, selected.length());

    mRope = mRope.replace(start, end - start, selected);

    processWordWrap();
}

void Document::set_text_color_selected(Color color) {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.setColor(color);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_text_color(Color color) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    nstring selected = mRope.subnstr(start, end - start);

    selected.setColor(color);

    mRope = mRope.replace(start, end - start, selected);
}

Color Document::get_text_color() const {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    return mRope[pos].getColor();
}

void Document::set_background_color_selected(Color color) {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.setBackgroundColor(color);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_background_color(Color color) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    nstring selected = mRope.subnstr(start, end - start);

    selected.setBackgroundColor(color);

    mRope = mRope.replace(start, end - start, selected);
}

Color Document::get_background_color() const {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    return mRope[pos].getBackgroundColor();
}

void Document::set_font_size_selected(int size) {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);

    selected.setFontSize(size);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_font_size(int size) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    nstring selected = mRope.subnstr(start, end - start);

    selected.setFontSize(size);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_font_id_selected(std::size_t id) {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    nstring selected = mRope.subnstr(start, end - start);
    selected.setFontId(id);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_font_id(std::size_t id) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    nstring selected = mRope.subnstr(start, end - start);
    selected.setFontId(id);

    mRope = mRope.replace(start, end - start, selected);
}

void Document::set_link_selected(std::string link) {
    save_snapshot();

    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);
    nstring selected = mRope.subnstr(start, end - start);
    selected.setLink(link);

    mRope = mRope.replace(start, end - start, selected);

    std::cout << link << " | " << selected.getLink() << " "
              << mRope[start].getLink() << std::endl;
}

void Document::set_link(std::string link) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    nstring selected = mRope.subnstr(start, end - start);
    selected.setLink(link);

    mRope = mRope.replace(start, end - start, selected);
}

std::string Document::get_link_selected() const {
    if (!is_selecting()) return std::string();
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);
    nstring selected = mRope.subnstr(start, end - start);

    return selected.getLink();
}

void Document::processWordWrap() {
    auto getFont = [&](const nchar& c) -> Font {
        if (c.isBold() && c.isItalic()) {
            return mDocFonts->get_bold_italic_font(c.getFontId());
        } else if (c.isBold()) {
            return mDocFonts->get_bold_font(c.getFontId());
        } else if (c.isItalic()) {
            return mDocFonts->get_italic_font(c.getFontId());
        }
        return mDocFonts->get_font(c.getFontId());
    };

    nstring content = mRope.to_nstring() + nstring("?");
    int length = content.length();

    bool wordWrap = 1;
    Rectangle rec = {0, 0,
                     constants::document::default_view_width -
                         2 * constants::document::margin_left,
                     constants::document::default_view_height -
                         2 * constants::document::margin_top};
    float spacing = 0.0f;

    displayPositions.clear();

    float textOffsetX = 0.0f;
    float textOffsetY = 0.0f;

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;  // Index where to begin drawing (where a line begins)
    int endLine = -1;    // Index where to stop drawing (where a line ends)
    int lastk = -1;      // Holds last value of the character position

    float curLineHeight = 0.0f;

    for (int i = 0, k = 0; i < length; i++, k++) {
        Font charFont = getFont(content[i]);
        const char* charText = content[i].getChar();

        // Vector2 charSize =
        //     utils::measure_text(charFont, content[i].getChar(), 36, 2);
        std::size_t charFontSize = content[i].getFontSize();

        if (content[i].isSuperscript() || content[i].isSubscript()) {
            charFontSize /= 2;
        }

        float scaleFactor = charFontSize / (float)charFont.baseSize;

        curLineHeight =
            std::max(curLineHeight,
                     (charFont.baseSize + charFont.baseSize / 2) * scaleFactor);

        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(charText, &codepointByteCount);
        int index = GetGlyphIndex(charFont, codepoint);

        if (codepoint == 0x3f) codepointByteCount = 1;
        codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n') {
            glyphWidth = (charFont.glyphs[index].advanceX == 0)
                             ? charFont.recs[index].width * scaleFactor
                             : charFont.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        if (state == MEASURE_STATE) {
            if ((codepoint == ' ') || (codepoint == '\t') ||
                (codepoint == '\n'))
                endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width) {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine)
                    endLine = (i - codepointByteCount);

                state = !state;
            } else if ((i + 1) == length) {
                endLine = i;
                state = !state;
            } else if (codepoint == '\n')
                state = !state;

            if (state == DRAW_STATE) {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        } else {
            if (codepoint == '\n') {
                displayPositions.push_back({textOffsetX, textOffsetY});
            } else {
                // When text overflows rectangle height limit, just stop
                // drawing
                if ((textOffsetY + charFont.baseSize * scaleFactor) >
                    rec.height)
                    break;

                int offsetY = charFont.baseSize * scaleFactor;

                displayPositions.push_back({
                    textOffsetX, textOffsetY
                    // + (content[i].isSubscript() ? offsetY : 0)
                });
            }

            if (wordWrap && (i == endLine)) {
                textOffsetY += curLineHeight;
                // textOffsetY +=
                //     (charFont.baseSize + charFont.baseSize / 2) *
                //     scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                k = lastk;

                state = !state;

                curLineHeight = 0.0f;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth;  // avoid leading spaces
    }
}