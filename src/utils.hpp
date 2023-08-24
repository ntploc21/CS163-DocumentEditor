#ifndef UTILS_HPP
#define UTILS_HPP

#include "raylib.h"

namespace utils {
    Vector2 measure_text(Font font, const char* text, int font_size,
                         int font_spacing);

    char* unicodeToChar(int unicode);

    // Draw text using font inside rectangle limits
    void DrawTextBoxed(Font font, const char* text, Rectangle rec,
                       float fontSize, float spacing, bool wordWrap,
                       Color tint);

    // Draw text using font inside rectangle limits with support for text
    // selection};
    void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec,
                                 float fontSize, float spacing, bool wordWrap,
                                 Color tint, int selectStart, int selectLength,
                                 Color selectTint, Color selectBackTint);
}  // namespace utils

#endif  // UTILS_HPP