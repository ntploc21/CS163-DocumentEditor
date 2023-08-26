#include "utils.hpp"

#include <algorithm>
#include <iostream>

#include "constants.hpp"

namespace utils {

    Vector2 measure_text(Font font, const char* text, int font_size,
                         int spacing) {
        return MeasureTextEx(font, text, font_size, spacing);
    }

#define BIT(x, i) (((x) >> (i)) & 1)

    char* unicodeToChar(int unicode) {
        char* c;

        if (unicode < 128) {
            c = new char[2];
            c[0] = unicode;
            c[1] = '\0';
        } else if (unicode < 2048) {
            c = new char[3];
            c[0] = 192 | (unicode >> 6);
            c[1] = 128 | (unicode & 63);
            c[2] = '\0';
        } else if (unicode < 65536) {
            c = new char[4];
            c[0] = 224 | (unicode >> 12);
            c[1] = 128 | ((unicode >> 6) & 63);
            c[2] = 128 | (unicode & 63);
            c[3] = '\0';
        } else {
            c = new char[5];
            c[0] = 240 | (unicode >> 18);
            c[1] = 128 | ((unicode >> 12) & 63);
            c[2] = 128 | ((unicode >> 6) & 63);
            c[3] = 128 | (unicode & 63);
            c[4] = '\0';
        }

        return c;
    }

    bool cmpVector2(const Vector2& a, const Vector2& b) {
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    }

    Vector2 get_init_pos() {
        return Vector2{
            1.0f * std::max(0, (GetScreenWidth() -
                                constants::document::default_view_width) /
                                   2) -
                1 + constants::document::padding_left,
            1.0f * constants::document::margin_top +
                constants::document::padding_top};
    }

    Vector2 sum(Vector2 a, Vector2 b) { return Vector2{a.x + b.x, a.y + b.y}; }

}  // namespace utils