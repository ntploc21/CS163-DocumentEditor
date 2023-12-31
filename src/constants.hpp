#pragma once
#include <string>

#include "raylib.h"

// Global variables
namespace constants::window {
    constexpr int width = 1300;
    constexpr int height = 800;
    const std::string title = "LocDocs";

    // const std::string favicon = "assets/images/favicon.png";
}  // namespace constants::window

namespace constants::document {
    constexpr int default_width = 816;
    constexpr int default_height = 1056;

    constexpr int default_view_width = 1200;
    constexpr int default_view_height = 1553;

    constexpr int margin_top = 50;
    constexpr int margin_left = 50;

    constexpr int padding_top = 50;
    constexpr int padding_left = 50;

    constexpr int outline_margin_left = 30;
    constexpr int outline_margin_right = 30;

    constexpr int default_font_size = 36;

    constexpr int default_font_id = 0;

    constexpr Color default_text_color = BLACK;

    constexpr Color default_background_color = WHITE;

}  // namespace constants::document

namespace constants::dictionary {
    constexpr int char_limit = (1 << 8);
    enum locale_language {
        ENGLISH = 0,
        VIETNAMESE = 1,

        NUM_LANGUAGES = 2,
    };
    constexpr locale_language default_language = locale_language::ENGLISH;
    const std::string default_database_path =
        "data/dictionary/english/words.txt";
}  // namespace constants::dictionary

namespace constants::keyboard {
    constexpr int char_limit = 336;
}