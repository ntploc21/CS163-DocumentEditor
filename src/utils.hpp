#ifndef UTILS_HPP
#define UTILS_HPP

#include "raylib.h"

namespace utils {
    Vector2 measure_text(Font font, const char* text, int font_size,
                         int font_spacing);

    char* unicodeToChar(int unicode);

    bool cmpVector2(const Vector2& a, const Vector2& b);
}  // namespace utils

#endif  // UTILS_HPP