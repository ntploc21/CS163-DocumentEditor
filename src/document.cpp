#include "document.hpp"

Document::Document() {}

Document::Document(std::string filename) : mFilename{filename} {
    // not implemented yet
}

Rope& Document::rope() { return mRope; }

const Rope& Document::rope() const { return mRope; }

Cursor& Document::cursor() { return mCursor; }

const Cursor& Document::cursor() const { return mCursor; }

void Document::set_cursor(Cursor cursor) { mCursor = cursor; }

std::string& Document::filename() { return mFilename; }

const std::string& Document::filename() const { return mFilename; }

void Document::cursor_move_next_char() {
    if (mCursor.column < mRope.line_length(mCursor.line)) {
        ++mCursor.column;
    } else if (mCursor.line < mRope.line_count() - 1) {
        ++mCursor.line;
        mCursor.column = 0;
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

void Document::insert_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }
}

void Document::append_at_cursor(const nstring& text) {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    mRope = mRope.insert(pos, text);

    for (std::size_t i = 0; i < text.length(); ++i) {
        cursor_move_next_char();
    }
}

void Document::erase_at_cursor() {
    std::size_t pos = mRope.index_from_pos(mCursor.line, mCursor.column);
    if (pos == 0) return;

    cursor_move_prev_char();
    mRope = mRope.erase(pos - 1, 1);
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
}

void Document::redo() {
    if (mRedo.empty()) return;

    mUndo.push_back({mRope, mCursor});
    const auto& [next_rope, next_cursor] = mRedo.back();

    mRope = next_rope;
    set_cursor(next_cursor);

    mRedo.pop_back();
}