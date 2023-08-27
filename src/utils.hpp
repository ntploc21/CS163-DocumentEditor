#ifndef UTILS_HPP
#define UTILS_HPP

#include "raylib.h"
#include "text/nstring.hpp"

namespace utils {
    Vector2 measure_text(Font font, const char* text, int font_size,
                         int font_spacing);

    char* unicodeToChar(int unicode);

    bool cmpVector2(const Vector2& a, const Vector2& b);

    Vector2 get_init_pos();

    Vector2 sum(Vector2 a, Vector2 b);

    void open_link(nstring link);

    std::size_t number_length(std::size_t number);

    // Draw textbox with specified position and size
    // bool TextBox(Rectangle bounds, nstring& text, int textSize, bool
    // editMode);

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