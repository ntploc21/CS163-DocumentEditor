#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <cstddef>

struct Cursor {
    int line{};
    int column{};
    int index{};

    bool operator==(const Cursor& other) const {
        return line == other.line && column == other.column &&
               index == other.index;
    }
    bool operator!=(const Cursor& other) const { return !(*this == other); }
    bool operator<(const Cursor& other) const {
        return line < other.line ||
               (line == other.line && column < other.column);
    }
    bool operator>(const Cursor& other) const {
        return line > other.line ||
               (line == other.line && column > other.column);
    }
    bool operator<=(const Cursor& other) const {
        return line <= other.line ||
               (line == other.line && column <= other.column);
    }
    bool operator>=(const Cursor& other) const {
        return line >= other.line ||
               (line == other.line && column >= other.column);
    }
};

#endif  // CURSOR_HPP