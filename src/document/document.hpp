#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <optional>

#include "DocumentFont.hpp"
#include "FontFactory.hpp"
#include "cursor.hpp"
#include "dictionary/dictionary.hpp"
#include "raylib.h"
#include "rope/rope.hpp"

class Document {
public:
    Document();
    Document(std::string filename);

    void set_font_factory(FontFactory* fonts);
    void set_document_fonts(DocumentFont* docFonts);

    void set_dictionary(Dictionary* dictionary);

    Rope& rope();
    const Rope& rope() const;

    Cursor& cursor();
    const Cursor& cursor() const;
    void set_cursor(Cursor cursor);

    std::string& filename();
    const std::string& filename() const;

    Cursor& select_orig();
    const Cursor& select_orig() const;
    const Cursor& select_start() const;
    const Cursor& select_end() const;

    void cursor_move_line(int delta);

    void cursor_move_column(int delta);
    // void cursor_move_prev_line_wrap();
    // void cursor_move_next_line_wrap();

    void cursor_move_next_char();
    void cursor_move_prev_char();
    void cursor_move_next_word();
    void cursor_move_prev_word();

    Cursor pos_on_mouse() const;

    void insert_at_cursor(const nstring& text);
    void append_at_cursor(const nstring& text);
    void erase_at_cursor();
    void erase_selected();
    void erase_range(std::size_t start, std::size_t end);
    void copy_selected();
    void copy_range(std::size_t start, std::size_t end);

    void save_snapshot();
    void undo();
    void redo();
    // std::optional< Rope > undo_top();
    // void save();
    // void save_as();

    Vector2 get_display_positions(std::size_t index) const;

    void turn_on_selecting();
    void turn_off_selecting();
    bool is_selecting() const;

public:
    bool check_word_at_cursor();
    std::vector< nstring > suggest_at_cursor();

public:
    // pair< heading type, heading text >
    std::vector< std::pair< std::size_t, nstring > > get_outline() const;

public:
    void underline_selected();
    void strikethrough_selected();
    void bold_selected();
    void italic_selected();
    void subscript_selected();
    void superscript_selected();

    void set_text_color_selected(Color color);

    void set_text_color(Color color);

    Color get_text_color() const;

    void set_background_color_selected(Color color);

    void set_background_color(Color color);

    Color get_background_color() const;

    void set_font_size_selected(int size);

    void set_font_size(int size);

    void set_font_id_selected(std::size_t id);

    void set_font_id(std::size_t id);

    void set_link_selected(std::string link);

    void set_link(std::string link);

    std::string get_link_selected() const;

private:
    void processWordWrap();
    // void processWordWrap2();

private:
    struct Snapshot {
        Rope rope;
        Cursor cursor;
    };
    Rope mRope{};

    std::vector< Snapshot > mUndo{};
    std::vector< Snapshot > mRedo{};

    Cursor mCursor{};
    Cursor mSelectOrig{-1, -1};

    std::vector< Vector2 > displayPositions{};
    std::vector< bool > validWords{};

    std::string mFilename{"Untitled"};

    Dictionary* mDictionary;

    FontFactory* mFonts;
    DocumentFont* mDocFonts;

    bool mIsSelecting{false};

private:
    Color mTextColor{constants::document::default_text_color};
    Color mBackgroundColor{constants::document::default_background_color};
};

#endif  // DOCUMENT_HPP