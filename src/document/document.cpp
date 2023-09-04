#include "document.hpp"

#include <algorithm>
#include <fstream>
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

    std::cout << mCursor.line << " " << mCursor.column << std::endl;

    cursor_move_column(0);
}

void Document::cursor_move_column(int delta) {
    int lineLength = mRope.line_length(mCursor.line);

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

    if (line == mRope.line_count()) {
        column = mRope.line_length(--line);
    }

    return Cursor{static_cast< int >(line), static_cast< int >(column)};
}

void Document::insert_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    if (text.length() == 1 &&
        (text[0].codepoint() == int('\n') || text[0].codepoint() == int(' '))) {
        save_snapshot();

        if (text[0].codepoint() == int('\n')) {
            mDocumentStructure.insert_line(mCursor.line + 1);
        }
    }

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }

    processWordWrap();
}

void Document::append_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    if (text.length() == 1 &&
        (text[0].codepoint() == int('\n') || text[0].codepoint() == int(' '))) {
        save_snapshot();

        if (text[0].codepoint() == int('\n')) {
            mDocumentStructure.insert_line(mCursor.line + 1);
        }
    }

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }

    processWordWrap();
}

void Document::erase_at_cursor() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    if (pos == 0) return;

    cursor_move_prev_char();

    if (mRope[pos - 1].codepoint() == int('\n')) {
        mDocumentStructure.delete_line(mCursor.line + 1);
    }

    mRope = mRope.erase(pos - 1, 1);

    processWordWrap();
}

void Document::erase_selected() {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    // delete lines between start and end
    std::size_t start_line = select_start().line + 1;
    std::size_t end_line = select_end().line + 1;
    mDocumentStructure.delete_lines(start_line, end_line);

    erase_range(start, end);
}

void Document::erase_range(std::size_t start, std::size_t end) {
    save_snapshot();

    // delete lines between start and end
    std::size_t start_line = mRope.pos_from_index(start).first + 1;
    std::size_t end_line = mRope.pos_from_index(end).first + 1;
    mDocumentStructure.delete_lines(start_line, end_line);

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
    mUndo.push_back({mRope, mDocumentStructure.rope(), mCursor});
    mRedo.clear();
}

void Document::undo() {
    if (mUndo.empty()) return;

    mRedo.push_back({mRope, mDocumentStructure.rope(), mCursor});
    const auto& [prev_rope, prev_structure, prev_cursor] = mUndo.back();

    mRope = prev_rope;
    mDocumentStructure.rope() = prev_structure;
    set_cursor(prev_cursor);

    mUndo.pop_back();

    processWordWrap();
}

void Document::redo() {
    if (mRedo.empty()) return;

    mUndo.push_back({mRope, mDocumentStructure.rope(), mCursor});
    const auto& [next_rope, prev_structure, next_cursor] = mRedo.back();

    mRope = next_rope;
    mDocumentStructure.rope() = prev_structure;
    set_cursor(next_cursor);

    mRedo.pop_back();

    processWordWrap();
}

void Document::save() {
    if (mFilename.empty() || mFilename == "Untitled") {
        std::string filename =
            utils::open_file_dialog("Save File", "", {"*.txt"}, "", false);
        if (filename.empty()) return;
        mFilename = filename;
    }

    std::ofstream file(mFilename);
    std::size_t numLine = mRope.line_count();
    for (std::size_t i = 0; i < numLine; ++i) {
        file << mRope.subnstr(mRope.index_from_pos(i, 0), mRope.line_length(i))
             << "\n";
    }

    for (std::size_t i = 0; i < numLine; ++i) {
        file << mDocumentStructure.get_headings(i);
        file << mDocumentStructure.get_alignment(i);
        file << mDocumentStructure.get_list(i);
        file << "\n";
    }

    for (std::size_t i = 0; i < mRope.length(); ++i) {
        file << mRope[i].getColor().r << " " << mRope[i].getColor().g << " "
             << mRope[i].getColor().b << " " << mRope[i].getColor().a << " ";

        file << mRope[i].getBackgroundColor().r << " "
             << mRope[i].getBackgroundColor().g << " "
             << mRope[i].getBackgroundColor().b << " "
             << mRope[i].getBackgroundColor().a << " ";

        file << mRope[i].getFontSize() << " ";

        file << mRope[i].getFontId() << " ";

        file << (mRope[i].hasLink() ? mRope[i].getLink() : "-") << " ";

        file << mRope[i].getType() << " ";

        file << "\n";
    }

    std::vector< std::string > fonts = mDocFonts->getFontList();

    file << fonts.size() << "\n";

    for (std::size_t i = 0; i < fonts.size(); ++i) {
        file << fonts[i] << "|" << mFonts->getPath(fonts[i]) << "\n";
    }

    file.close();
}

void Document::load(std::string filename) {
    if (filename.empty()) return;
}

void Document::loadFont(const std::string& fontName, const std::string& path) {
    std::string extension = path.substr(path.find_last_of(".") + 1);
    if (extension != "ttf") return;

    std::string name = path.substr(path.find_last_of("/") + 1);

    std::string dir = path.substr(0, path.find_last_of("/"));

    std::string boldPath = dir + "/" + name + " bold.ttf";
    std::string italicPath = dir + "/" + name + " italic.ttf";
    std::string boldItalicPath = dir + "/" + name + " bold italic.ttf";

    mFonts->Load(fontName, path);
    mFonts->Load(fontName + " Bold", boldPath);
    mFonts->Load(fontName + " Italic", italicPath);
    mFonts->Load(fontName + " Bold Italic", boldItalicPath);

    FontInfo info{
        fontName,
        fontName + " Bold",
        fontName + " Italic",
        fontName + " Bold Italic",
    };
    mDocFonts->registerFont(info);
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
    nstring word = get_word_at_cursor();
    return mDictionary->search(word);
}

std::vector< nstring > Document::suggest_at_cursor() {
    nstring word = get_word_at_cursor();
    return mDictionary->suggest(word);
}

nstring Document::get_word_at_cursor() const {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    return mRope.subnstr(left, right - left);
}

nstring Document::get_selected_text() const {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);
    std::size_t end =
        mRope.index_from_pos(select_end().line, select_end().column);

    return mRope.subnstr(start, end - start);
}

void Document::replace_word_at_cursor(const nstring& text) {
    save_snapshot();

    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    int left = pos, right = pos;

    while (left > 0 && std::isalnum(mRope[left - 1].codepoint())) --left;
    while (right < mRope.length() && std::isalnum(mRope[right].codepoint()))
        ++right;

    int start = left, end = right;

    mRope = mRope.replace(start, end - start, text);

    cursor().column += left - pos + text.length();

    turn_off_selecting();

    processWordWrap();
}

std::vector< std::pair< std::size_t, std::size_t > > Document::get_outline()
    const {
    std::vector< std::pair< std::size_t, std::size_t > > headings;

    for (std::size_t i = 0; i < mRope.line_count(); ++i) {
        std::size_t heading = mDocumentStructure.get_headings(i);
        if (heading == Heading::None) continue;

        headings.push_back({heading, i});
    }

    return headings;
}

nstring Document::get_line_text(std::size_t line_idx) const {
    std::size_t start = mRope.index_from_pos(line_idx, 0);
    std::size_t end = mRope.index_from_pos(line_idx + 1, 0);

    return mRope.subnstr(start, end - start);
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

int Document::get_font_size() const {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    return mRope[pos].getFontSize();
}

int Document::get_font_size_selected() const {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);

    return mRope[start].getFontSize();
}

void Document::set_line_font_size(std::size_t line_idx, int size) {
    std::size_t start = mRope.index_from_pos(line_idx, 0);
    std::size_t end = mRope.index_from_pos(line_idx + 1, 0);

    nstring selected = mRope.subnstr(start, end - start);

    std::size_t font_size = size;

    selected.setFontSize(font_size);

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

std::size_t Document::get_font_id() const {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);

    return mRope[pos].getFontId();
}

std::size_t Document::get_font_id_selected() const {
    std::size_t start =
        mRope.index_from_pos(select_start().line, select_start().column);

    return mRope[start].getFontId();
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

void Document::align_selected(Alignment align) {
    save_snapshot();

    for (std::size_t i = select_start().line; i <= select_end().line; ++i) {
        mDocumentStructure.set_alignment(i, align);
    }

    processWordWrap();
}

void Document::align_current_line(Alignment align) {
    save_snapshot();

    mDocumentStructure.set_alignment(mCursor.line, align);

    processWordWrap();
}

void Document::set_heading_selected(Heading heading) {
    save_snapshot();

    for (std::size_t i = select_start().line; i <= select_end().line; ++i) {
        set_line_font_size(i, constants::document::heading_font_size[heading]);
        mDocumentStructure.set_headings(i, heading);
    }

    processWordWrap();
}

void Document::set_heading_current_line(Heading heading) {
    save_snapshot();

    // set font size of all characters on current line
    set_line_font_size(mCursor.line,
                       constants::document::heading_font_size[heading]);

    // set heading of current line
    mDocumentStructure.set_headings(mCursor.line, heading);

    processWordWrap();
}

void Document::set_list_selected(List list) {
    save_snapshot();

    for (std::size_t i = select_start().line; i <= select_end().line; ++i) {
        mDocumentStructure.set_list(i, list);
    }

    processWordWrap();
}

void Document::set_list_current_line(List list) {
    save_snapshot();

    mDocumentStructure.set_list(mCursor.line, list);

    processWordWrap();
}

std::size_t Document::get_list(std::size_t line_idx) const {
    return mDocumentStructure.get_list(line_idx);
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

    nstring content = mRope.to_nstring();

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

    std::size_t cur_line_idx = 0;
    int line_start = mRope.find_line_start(cur_line_idx);
    int next_line_start;
    int length = content.length();

    std::size_t listNumberCounter = 0;

    std::cout << "line count: " << mRope.line_count() << std::endl;

    for (; cur_line_idx < mRope.line_count();
         cur_line_idx++, line_start = next_line_start) {
        next_line_start = mRope.find_line_start(cur_line_idx + 1);

        nstring lineContent =
            mRope.subnstr(line_start, next_line_start - line_start) +
            nstring("?");

        int line_length = next_line_start - line_start;
        float curLineHeight = 0.0f;

        int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

        std::size_t align = mDocumentStructure.get_alignment(cur_line_idx);
        std::size_t listType = mDocumentStructure.get_list(cur_line_idx);

        int x_indent =
            (listType == List::None ? 0 : constants::document::list_indent);

        if (listType == List::Number) {
            std::string idx = std::to_string(++listNumberCounter) + ".";

            int widthCounter =
                utils::measure_text(mFonts->Get("Arial"), idx.c_str(), 36, 2.0f)
                    .x;

            x_indent = std::max(widthCounter + 30, x_indent);

        } else {
            listNumberCounter = 0;
        }

        textOffsetX = x_indent;

        std::vector< Vector2 > newDisplayPositions;

        int startLine =
            -1;  // Index where to begin drawing (where a line begins)
        int endLine = -1;
        // line_start - 1;  // Index where to stop drawing (where a line ends)
        int lastk = -1;  // Holds last value of the character position

        for (int i = 0, k = 0; i < lineContent.length(); i++, k++) {
            Font charFont = getFont(lineContent[i]);
            const char* charText = lineContent[i].getChar();

            std::size_t charFontSize = lineContent[i].getFontSize();

            if (lineContent[i].isSuperscript() ||
                lineContent[i].isSubscript()) {
                charFontSize /= 2;
            }

            float scaleFactor = charFontSize / (float)charFont.baseSize;

            curLineHeight = std::max(
                curLineHeight,
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
                glyphWidth =
                    (charFont.glyphs[index].advanceX == 0)
                        ? charFont.recs[index].width * scaleFactor
                        : charFont.glyphs[index].advanceX * scaleFactor;

                if (i + 1 < line_length) glyphWidth = glyphWidth + spacing;
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
                } else if ((i + 1) == line_length) {
                    endLine = i;
                    state = !state;
                } else if (codepoint == '\n')
                    state = !state;

                if (state == DRAW_STATE) {
                    textOffsetX = x_indent;
                    i = startLine;
                    glyphWidth = 0;

                    // Save character position when we switch states
                    int tmp = lastk;
                    lastk = k - 1;
                    k = tmp;
                }
            } else {
                if (codepoint == '\n') {
                    newDisplayPositions.push_back({textOffsetX, textOffsetY});
                } else {
                    if ((textOffsetY + charFont.baseSize * scaleFactor) >
                        rec.height)
                        break;

                    int offsetY = charFont.baseSize * scaleFactor;

                    newDisplayPositions.push_back({textOffsetX, textOffsetY});
                }

                if (wordWrap && (i == endLine)) {
                    textOffsetY += curLineHeight;
                    textOffsetX = x_indent;
                    startLine = endLine;
                    endLine = -1;
                    glyphWidth = 0;
                    k = lastk;

                    state = !state;

                    curLineHeight = 0.0f;
                }
            }

            if ((textOffsetX != x_indent) || (codepoint != ' '))
                textOffsetX += glyphWidth;  // avoid leading spaces
        }

        // textOffsetY += curLineHeight;

        int i = newDisplayPositions.size() - 1;
        for (; i > 0 && align; --i) {
            Vector2 pos = newDisplayPositions[i - 1];
            if (pos.y == newDisplayPositions.back().y) continue;
            break;
        }

        for (; i < newDisplayPositions.size(); ++i) {
            Vector2 pos = newDisplayPositions[i];

            // calculate character width
            Font charFont = getFont(content[next_line_start - 1]);
            const char* charText = content[next_line_start - 1].getChar();

            std::size_t charFontSize =
                content[next_line_start - 1].getFontSize();

            if (content[next_line_start - 1].isSuperscript() ||
                content[next_line_start - 1].isSubscript()) {
                charFontSize /= 2;
            }

            float width =
                utils::measure_text(charFont, charText, charFontSize, 2).x;

            float lineWidth = newDisplayPositions.back().x + width - pos.x;

            float offsetX = 0;

            if (align == Alignment::Center) {
                offsetX = (rec.width - x_indent - lineWidth) / 2;
            } else if (align == Alignment::Right) {
                offsetX = rec.width - x_indent - lineWidth;
            }

            for (int j = i; j < newDisplayPositions.size(); ++j) {
                newDisplayPositions[j].x += offsetX;
            }

            break;
        }

        displayPositions.insert(displayPositions.end(),
                                newDisplayPositions.begin(),
                                newDisplayPositions.end());
    }
    displayPositions.push_back({textOffsetX, textOffsetY});
}
