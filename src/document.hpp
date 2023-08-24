#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <optional>

#include "cursor.hpp"
#include "raylib.h"
#include "rope/rope.hpp"

class Document {
public:
    Document();
    Document(std::string filename);

    Rope& rope();
    const Rope& rope() const;

    Cursor& cursor();
    const Cursor& cursor() const;
    void set_cursor(Cursor cursor);

    std::string& filename();
    const std::string& filename() const;

    // void cursor_move_prev_line();
    // void cursor_move_next_line();
    // void cursor_move_prev_line_wrap();
    // void cursor_move_next_line_wrap();

    void cursor_move_next_char();
    void cursor_move_prev_char();
    // void cursor_move_next_word();
    // void cursor_move_prev_word();

    void insert_at_cursor(const nstring& text);
    void append_at_cursor(const nstring& text);
    void erase_at_cursor();
    // void erase_selected();
    // void erase_range(std::size_t start, std::size_t end);
    // void copy_selected();
    // void copy_range(std::size_t start, std::size_t end);

    void save_snapshot();
    void undo();
    void redo();
    // std::optional< Rope > undo_top();
    // void save();
    // void save_as();

private:
    struct Snapshot {
        Rope rope;
        Cursor cursor;
    };

    std::vector< Snapshot > mUndo{};
    std::vector< Snapshot > mRedo{};

    Cursor mCursor{};
    Rope mRope{};

    std::string mFilename{"Untitled"};
};

#endif  // DOCUMENT_HPP