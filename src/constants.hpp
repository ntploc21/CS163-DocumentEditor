#pragma once
#include <string>

// Global variables
namespace constants::window {
    constexpr int width = 1300;
    constexpr int height = 800;
    const std::string title = "Document Editor";

    // const std::string favicon = "assets/images/favicon.png";
}  // namespace constants::window

namespace constants::dictionary {
    constexpr int char_limit = (1 << 8);
    enum locale_language {
        ENGLISH = 0,
        VIETNAMESE = 1,

        NUM_LANGUAGES = 2,
    };
    constexpr locale_language default_language = locale_language::ENGLISH;
}  // namespace constants::dictionary

namespace constants::keyboard {
    constexpr int char_limit = 336;
}