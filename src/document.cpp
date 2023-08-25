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

void Document::processWordWrap() {
    float fontSize = 36.0f;
    nstring content = mRope.to_nstring() + nstring("?");

    bool wordWrap = 1;
    Font font = mFonts->Get("Arial");
    Rectangle rec = {0, 0,
                     constants::document::default_view_width -
                         2 * constants::document::margin_left,
                     constants::document::default_view_height -
                         2 * constants::document::margin_top};
    float spacing = 0.0f;

    displayPositions.clear();

    int length = content.length();  // Total length in bytes of the text,
                                    // scanned by codepoints in loop

    float textOffsetY = 0.0f;  // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;  // Offset X to next character to draw

    float scaleFactor =
        fontSize / (float)font.baseSize;  // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;  // Index where to begin drawing (where a line begins)
    int endLine = -1;    // Index where to stop drawing (where a line ends)
    int lastk = -1;      // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++) {
        const char* text = content[i].getChar();

        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(text, &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad
        // byte is found (and return 0x3f) but we need to draw all of the
        // bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n') {
            glyphWidth = (font.glyphs[index].advanceX == 0)
                             ? font.recs[index].width * scaleFactor
                             : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text
        // we can draw before going outside of the rec container We store
        // this info in startLine and endLine, then we change states, draw
        // the text between those two variables and change states again and
        // again recursively until the end of the text (or until we get
        // outside of the container). When wordWrap is OFF we don't need the
        // measure state so we go to the drawing state immediately and begin
        // drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE) {
            // TODO: There are multiple types of spaces in UNICODE, maybe
            // it's a good idea to add support for more Ref:
            // http://jkorpela.fi/chars/spaces.html
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
                if (!wordWrap) {
                    textOffsetY +=
                        (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                displayPositions.push_back({textOffsetX, textOffsetY});
            } else {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width)) {
                    textOffsetY +=
                        (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop
                // drawing
                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
                    break;

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t')) {
                    // displayPositions.push_back({textOffsetX,
                    // textOffsetY});
                }

                displayPositions.push_back({textOffsetX, textOffsetY});
            }

            if (wordWrap && (i == endLine)) {
                textOffsetY +=
                    (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth;  // avoid leading spaces
    }
}
