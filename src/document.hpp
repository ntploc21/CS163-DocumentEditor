#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <optional>

#include "raylib.h"
#include "rope/rope.hpp"

class Document {
public:
    Document();
    Document(std::string filename);
    ~Document();

    std::string& filename();
    const std::string& filename() const;

    void cursor_move_prev_line();
    void cursor_move_next_line();
    void cursor_move_prev_line_wrap();
    void cursor_move_next_line_wrap();

    void cursor_move_next_char();
    void cursor_move_prev_char();
    void cursor_move_next_word();
    void cursor_move_prev_word();

    void insert_at_cursor(const std::string& text);
    void append_at_cursor(const std::string& text);
    void erase_at_cursor();
    void erase_selected();
    void erase_range(std::size_t start, std::size_t end);
    void copy_selected();
    void copy_range(std::size_t start, std::size_t end);

    void undo();
    std::optional< Rope > undo_top();
    void redo();
    void save_snapshot();
    void save();
    void save_as();

private:
    Rope mRope;
    std::string mFilename{"Untitled"};
};

#endif  // DOCUMENT_HPP