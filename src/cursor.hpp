#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <cstddef>

struct Cursor {
    std::size_t line{};
    std::size_t column{};
    std::size_t index{};

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