#include "editor.hpp"

#include <string.h>

#include <iostream>
#include <locale>
#include <set>
#include <vector>

#include "clip.h"
#include "constants.hpp"
#include "raygui.h"
#include "rope/rope.hpp"
#include "utils.hpp"

void Editor::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(constants::window::width, constants::window::height,
               constants::window::title.c_str());

    SetExitKey(KEY_NULL);
    MaximizeWindow();

    LoadResources();

    currentDocument().set_font_factory(fonts);
    currentDocument().set_document_fonts(mDocumentFont);

    currentDocument().set_dictionary(mDictionary);

    mDocumentFont->set_font_factory(fonts);
    PrepareKeybinds();
}

bool Editor::WindowClosed() { return closed; }

void Editor::Close() { CloseWindow(); }

void Editor::Run() {
    /* render */
    Update(GetFrameTime());
    Render();

    mKeybind.process(mMode == EditorMode::Insert);

    if (WindowShouldClose()) closed = true;
}

void Editor::Render() {
    BeginDrawing();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Color{249, 251, 253, 255});

    DrawOutline();

    DrawEditor();

    DrawPage();

    EndDrawing();
}

std::vector< int > s;
// std::string tmp;
// Rope tmp;

void Editor::Update([[maybe_unused]] float dt) {
    switch (mMode) {
        case EditorMode::Normal:
            NormalMode();
            break;
        case EditorMode::Insert:
            InsertMode();
            break;
        case EditorMode::Search:
            SearchMode();
            break;
    }
}

void Editor::DrawEditor() {
    int documentWidth = constants::document::default_view_width;
    int documentHeight = constants::document::default_view_height;
    int margin_top = constants::document::margin_top;

    DrawRectangle(std::max(0, (GetScreenWidth() - documentWidth) / 2 - 1),
                  margin_top - 1, documentWidth + 2, documentHeight + 2,
                  LIGHTGRAY);

    DrawRectangle(std::max(0, (GetScreenWidth() - documentWidth) / 2),
                  margin_top, documentWidth, documentHeight, WHITE);

    DrawEditorText();
}

void Editor::DrawEditorText() {
    auto getFont = [&](const nchar& c) -> Font {
        if (c.isBold() && c.isItalic()) {
            return mDocumentFont->get_bold_italic_font(c.getFontId());
        } else if (c.isBold()) {
            return mDocumentFont->get_bold_font(c.getFontId());
        } else if (c.isItalic()) {
            return mDocumentFont->get_italic_font(c.getFontId());
        }
        return mDocumentFont->get_font(c.getFontId());
    };

    int documentWidth = constants::document::default_view_width;
    int documentHeight = constants::document::default_view_height;
    int margin_top = constants::document::margin_top;
    // Vector2 char_size = utils::measure_text(" ", 20, 0);

    const auto& cursor = currentDocument().cursor();
    const auto& content = currentDocument().rope();

    std::size_t cur_line_idx = 0;
    std::size_t line_start = content.find_line_start(cur_line_idx);
    std::size_t next_line_start;

    for (; cur_line_idx < content.line_count();
         cur_line_idx++, line_start = next_line_start) {
        next_line_start = content.find_line_start(cur_line_idx + 1);

        float line_height = 0;
        float y = currentDocument().get_display_positions(line_start).y;

        for (std::size_t i = line_start; i < next_line_start - 1; ++i) {
            Vector2 charSize = utils::measure_text(fonts->Get("Arial"),
                                                   content[i].getChar(), 36, 2);
            line_height = std::max(line_height, charSize.y);
        }

        // draw text
        for (std::size_t i = line_start; i < next_line_start; ++i) {
            Font charFont = getFont(content[i]);
            std::size_t charFontSize = content[i].getFontSize();
            Color textColor = content[i].getColor();
            Color backgroundColor = content[i].getBackgroundColor();

            bool hasLink = content[i].hasLink();

            if (hasLink) textColor = BLUE;

            Vector2 pos = currentDocument().get_display_positions(i);
            Vector2 charSize = utils::measure_text(
                charFont, content[i].getChar(), charFontSize, 2);

            if (content[i].isSuperscript() || content[i].isSubscript()) {
                charSize.x /= 2, charSize.y /= 2;
                charFontSize /= 2;
            }

            if (content[i].isSubscript()) {
                pos.y += charSize.y;
            }

            // draw background
            DrawRectangle(utils::sum(utils::get_init_pos(), pos).x,
                          utils::sum(utils::get_init_pos(), pos).y, charSize.x,
                          charSize.y, backgroundColor);

            DrawTextEx(charFont, content[i].getChar(),
                       utils::sum(utils::get_init_pos(), pos), charFontSize, 2,
                       textColor);

            if (content[i].isUnderline()) {
                DrawLineEx(
                    utils::sum(utils::get_init_pos(),
                               Vector2{pos.x, pos.y + charSize.y}),
                    utils::sum(utils::get_init_pos(),
                               Vector2{pos.x + charSize.x, pos.y + charSize.y}),
                    1.5f, textColor);
            }

            if (content[i].isStrikethrough()) {
                DrawLineEx(
                    utils::sum(utils::get_init_pos(),
                               Vector2{pos.x, pos.y + 2 * charSize.y / 3}),
                    utils::sum(utils::get_init_pos(),
                               Vector2{pos.x + charSize.x,
                                       pos.y + 2 * charSize.y / 3}),
                    1.5f, textColor);
            }
        }

        // draw selected chars
        if (currentDocument().is_selecting()) {
            const auto& select_start = currentDocument().select_start();
            const auto& select_end = currentDocument().select_end();

            std::size_t select_start_idx =
                content.index_from_pos(select_start.line, select_start.column);
            std::size_t select_end_idx =
                content.index_from_pos(select_end.line, select_end.column);

            if (select_start.line <= cur_line_idx &&
                select_end.line >= cur_line_idx) {
                std::size_t start = std::max(select_start_idx, line_start);
                std::size_t end = std::min(select_end_idx, next_line_start - 1);

                for (std::size_t i = start; i < end; ++i) {
                    Font charFont = getFont(content[i]);
                    std::size_t charFontSize = content[i].getFontSize();

                    Vector2 pos = currentDocument().get_display_positions(i);
                    Vector2 charSize = utils::measure_text(
                        charFont, content[i].getChar(), charFontSize, 2);

                    if (content[i].isSuperscript() ||
                        content[i].isSubscript()) {
                        charSize.x /= 2, charSize.y /= 2;
                        charFontSize /= 2;
                    }

                    Vector2 rendered_pos =
                        utils::sum(utils::get_init_pos(), pos);

                    DrawRectangle(rendered_pos.x, rendered_pos.y,
                                  charSize.x + 1, line_height,
                                  ColorAlpha(GRAY, 0.3F));
                    //   Color{109, 158, 235, 255});
                }
            }
        }
    }

    // draw cursor block
    std::size_t cursor_pos = currentDocument().rope().index_from_pos(
        currentDocument().cursor().line, currentDocument().cursor().column);
    Vector2 cursor_display_pos =
        currentDocument().get_display_positions(cursor_pos);

    Vector2 cursor_rendered_pos =
        utils::sum(utils::get_init_pos(), cursor_display_pos);

    DrawRectangle(cursor_rendered_pos.x, cursor_rendered_pos.y, 1.5f, 36,
                  ORANGE);
}

void Editor::NormalMode() {}

void Editor::InsertMode() {
    static bool leftMousePrevDown = false;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Cursor cursor = currentDocument().pos_on_mouse();

        currentDocument().set_cursor(cursor);
        currentDocument().turn_off_selecting();
    } else {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (!leftMousePrevDown) {
                currentDocument().turn_on_selecting();
                currentDocument().select_orig() = currentDocument().cursor();
            }
            if (currentDocument().is_selecting()) {
                Cursor cursor = currentDocument().pos_on_mouse();

                currentDocument().set_cursor(cursor);

                setLinkPage(currentDocument().get_link_selected());
            }
            leftMousePrevDown = true;
        } else
            leftMousePrevDown = false;
    }

    // IsMouseButtonDown

    int key = GetCharPressed();
    if (!key) return;

    currentDocument().turn_off_selecting();

    if (key == '\n') {
        currentDocument().insert_at_cursor(nstring(nchar(key)));
        currentDocument().cursor_move_line(1);

        return;
    }

    if (key >= 128) currentDocument().erase_at_cursor();
    currentDocument().insert_at_cursor(nstring(nchar(key)));
}

void Editor::SearchMode() {}

Document& Editor::currentDocument() { return mDocument; }

const Document& Editor::currentDocument() const { return mDocument; }

void Editor::DrawOutline() {
    float documentWidth = constants::document::default_view_width;
    float documentHeight = constants::document::default_view_height;
    float margin_top = constants::document::margin_top;
    float margin_left = constants::document::outline_margin_left;
    float margin_right = constants::document::outline_margin_right;

    if ((GetScreenWidth() - documentWidth) / 2 - margin_right - margin_left <
        150)
        return;

    // draw outline on the left of the text editor like Google Docs
    DrawLineEx(
        Vector2{margin_left, margin_top},
        Vector2{margin_left +
                    std::max(0.0f, (GetScreenWidth() - documentWidth) / 2 -
                                       margin_right - margin_left),
                margin_top},
        2.0f, LIGHTGRAY);

    // draw title "Outline" with color #5f6368
    DrawTextEx(fonts->Get("Arial"), "Outline",
               Vector2{margin_left, margin_top - 22}, 24, 0,
               Color{95, 99, 104, 255});

    // draw outline
    std::vector< std::pair< std::size_t, nstring > > outline =
        currentDocument().get_outline();

    const std::size_t outline_heading_start_x[6] = {
        0,   // title
        0,   // 1
        20,  // 2
        40,  // 3
        60,  // 4
        80,  // 5
    };

    const std::size_t outline_heading_font_size[6] = {
        26,  // title
        26,  // 1
        24,  // 2
        22,  // 3
        20,  // 4
        18,  // 5
    };

    for (std::size_t i = 0; i < outline.size(); ++i) {
        std::size_t heading_type = outline[i].first;
        nstring heading_text = outline[i].second;

        float x = margin_left + outline_heading_start_x[heading_type];
        float y = margin_top + 20 + i * 50;
        std::size_t fontSize = outline_heading_font_size[heading_type];

        // draw heading text such that it not exceed the outline
        float cur_width = x - margin_left;
        for (std::size_t j = 0; j < heading_text.length(); ++j) {
            nchar c = heading_text[j];
            Vector2 charSize = utils::measure_text(fonts->Get("Arial"),
                                                   c.getChar(), fontSize, 0);

            cur_width += charSize.x;

            if (cur_width >
                std::max(0.0f, (GetScreenWidth() - documentWidth) / 2 -
                                   margin_right - margin_left) -
                    20) {
                heading_text = heading_text.substr(0, j) + nstring("...");
                break;
            }
        }

        DrawTextEx(fonts->Get("Arial"), heading_text.c_str(), Vector2{x, y},
                   fontSize, 0, Color{95, 99, 104, 255});
    }

    // check if user click on any heading
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        if (mousePos.x >= margin_left &&
            mousePos.x <=
                margin_left +
                    std::max(0.0f, (GetScreenWidth() - documentWidth) / 2 -
                                       margin_right - margin_left)) {
            if (mousePos.y >= margin_top + 20 &&
                mousePos.y <= margin_top + 20 + outline.size() * 50) {
                std::size_t heading_idx =
                    (mousePos.y - margin_top - 20) / 50 + 1;

                std::size_t heading_pos = outline[heading_idx - 1]
                                              .second.substr(0, 1)
                                              .to_string()[0] -
                                          '0';

                std::cout << heading_idx << std::endl;

                // std::size_t line_start =
                //     currentDocument().rope().find_line_start(heading_pos -
                //     1);

                // currentDocument().set_cursor(
                //     Cursor{static_cast< int >(line_start), 0});
            }
        }
    }
}

void Editor::DrawPage() {
    float documentWidth = constants::document::default_view_width;
    float documentHeight = constants::document::default_view_height;
    float margin_top = constants::document::margin_top;
    float margin_left = constants::document::outline_margin_left;
    float margin_right = constants::document::outline_margin_right;

    // draw the page right on the right of the text editor
    float initX = std::max(0.0f, (GetScreenWidth() - documentWidth) / 2 +
                                     documentWidth + margin_right);

    float initY = margin_top;

    switch (mPage) {
        case EditorPage::Link:
            DrawLinkPage(initX, initY);
            break;
        case EditorPage::Color:
            DrawColorPage(initX, initY);
            break;
        default:
            break;
    }
}

void Editor::SetPage(EditorPage page) { mPage = page; }

void Editor::DrawLinkPage(float initX, float initY) {
    // draw title "Link"
    DrawTextEx(fonts->Get("Arial"), "Link", Vector2{initX, initY - 22}, 24, 0,
               Color{95, 99, 104, 255});

    DrawLineEx(Vector2{initX, initY}, Vector2{(float)GetScreenWidth(), initY},
               2.0f, LIGHTGRAY);

    if (!currentDocument().is_selecting() ||
        currentDocument().select_start() == currentDocument().select_end()) {
        utils::DrawTextBoxed(
            fonts->Get("Arial"),
            "You must selecting a text you want to edit the link",
            Rectangle{initX, initY + 20, 300, 300}, 24, 0, false, RED);
        return;
    }
    char* url = new char[256];
    strcpy(url, currentURL.c_str());

    GuiTextBox(Rectangle{initX, initY + 20, 300, 50}, url, 256,
               (mMode == EditorMode::Normal));

    setLinkPage(url);

    bool openLink =
        GuiButton(Rectangle{initX, initY + 80, 300, 50}, "Open Link");
    if (openLink) utils::open_link(currentURL);

    bool clearLink =
        GuiButton(Rectangle{initX, initY + 140, 300, 50}, "Clear Link");
    if (clearLink) currentURL = "";

    bool saveLink =
        GuiButton(Rectangle{initX, initY + 200, 300, 50}, "Save Link");
    if (saveLink) {
        std::cout << currentURL << std::endl;
        currentDocument().save_snapshot();
        currentDocument().set_link_selected(currentURL);
    }

    delete[] url;
}

void Editor::setLinkPage(std::string url) { currentURL = url; }

void Editor::DrawColorPage(float initX, float initY) {
    // draw title "Color"
    DrawTextEx(fonts->Get("Arial"), "Color", Vector2{initX, initY - 22}, 24, 0,
               Color{95, 99, 104, 255});

    DrawLineEx(Vector2{initX, initY}, Vector2{(float)GetScreenWidth(), initY},
               2.0f, LIGHTGRAY);

    DrawTextEx(fonts->Get("Arial"), "Text Color", Vector2{initX, initY + 18},
               24, 0, Color{95, 99, 104, 255});
    currentColor = GuiColorPicker({initX, initY + 40, 300, 300}, currentColor);

    DrawTextEx(fonts->Get("Arial"), "Background Color",
               Vector2{initX, initY + 358}, 24, 0, Color{95, 99, 104, 255});
    currentBackgroundColor =
        GuiColorPicker({initX, initY + 380, 300, 300}, currentBackgroundColor);

    bool saveColor =
        GuiButton(Rectangle{initX, initY + 780, 300, 50}, "Save Color");
    if (saveColor) {
        currentDocument().save_snapshot();
        if (currentDocument().is_selecting()) {
            currentDocument().set_text_color_selected(currentColor);
            currentDocument().set_background_color_selected(
                currentBackgroundColor);
        } else {
            currentDocument().set_text_color(currentColor);
            currentDocument().set_background_color(currentBackgroundColor);
        }
    }
}

void Editor::LoadResources() {
    fonts->Load("Arial", "assets/fonts/SVN-Arial 3.ttf");
    fonts->Load("Arial Bold", "assets/fonts/SVN-Arial 3 bold.ttf");
    fonts->Load("Arial Italic", "assets/fonts/SVN-Arial 3 italic.ttf");
    fonts->Load("Arial Bold Italic",
                "assets/fonts/SVN-Arial 3 bold italic.ttf");
    FontInfo info{
        "Arial",
        "Arial Bold",
        "Arial Italic",
        "Arial Bold Italic",
    };

    mDocumentFont->registerFont(info);

    // mDictionary->loadDatabase(constants::dictionary::default_database_path);
}

void Editor::PrepareKeybinds() {
    // delete
    mKeybind.insert(
        {KEY_BACKSPACE},
        [&]() {
            if (currentDocument().is_selecting()) {
                currentDocument().erase_selected();
                currentDocument().turn_off_selecting();
                return;
            }
            currentDocument().turn_off_selecting();
            currentDocument().erase_at_cursor();
        },
        true);

    // delete
    mKeybind.insert(
        {KEY_DELETE},
        [&]() {
            if (currentDocument().is_selecting()) {
                currentDocument().erase_selected();
                currentDocument().turn_off_selecting();
                return;
            }
            currentDocument().turn_off_selecting();
            currentDocument().erase_at_cursor();
        },
        true);

    // undo
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_Z},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().undo();
        },
        true);

    // redo
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_Y},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().redo();
        },
        true);

    // new line
    mKeybind.insert(
        {KEY_ENTER},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().insert_at_cursor(nstring("\n"));
            // currentDocument().cursor_move_next_line();
        },
        true);

    // move left one char
    mKeybind.insert(
        {KEY_LEFT},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_prev_char();
        },
        true);

    // move right one char
    mKeybind.insert(
        {KEY_RIGHT},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_next_char();
        },
        true);

    // move up one line
    mKeybind.insert(
        {KEY_UP},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_line(-1);
        },
        true);

    // move down one line
    mKeybind.insert(
        {KEY_DOWN},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_line(1);
        },
        true);

    // move to the next word
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_RIGHT},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_next_word();
        },
        true);

    // move to the previous word
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT},
        [&]() {
            currentDocument().turn_off_selecting();
            currentDocument().cursor_move_prev_word();
        },
        true);

    // select the next word
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_RIGHT},
        [&]() {
            if (!currentDocument().is_selecting()) {
                currentDocument().turn_on_selecting();
                currentDocument().select_orig() = currentDocument().cursor();
            }
            currentDocument().cursor_move_next_word();
        },
        true);

    // select the previous word
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_LEFT},
        [&]() {
            if (!currentDocument().is_selecting()) {
                currentDocument().turn_on_selecting();
                currentDocument().select_orig() = currentDocument().cursor();
            }
            currentDocument().cursor_move_prev_word();
        },
        true);

    // move to the start of the line
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_RIGHT},
        [&]() {
            currentDocument().turn_on_selecting();
            currentDocument().select_orig() = currentDocument().cursor();
            currentDocument().cursor_move_next_word();
        },
        true);

    // copy
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_C},
        [&]() {
            currentDocument().save_snapshot();
            currentDocument().copy_selected();
        },
        true);

    // cut
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_X},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().copy_selected();
            currentDocument().erase_selected();
            currentDocument().turn_off_selecting();
        },
        true);

    // paste
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_V},
        [&]() {
            currentDocument().save_snapshot();

            std::string text = GetClipboardText();
            if (currentDocument().is_selecting()) {
                currentDocument().erase_selected();
                currentDocument().turn_off_selecting();
            }

            currentDocument().insert_at_cursor(text);
        },
        true);

    /* PARAGRAPH STYLE */
    // underline
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_U},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().underline_selected();
        },
        true);

    // strikethrough
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_F2},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().strikethrough_selected();
        },
        true);

    // bold
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_B},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().bold_selected();
        },
        true);
    // italic
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_I},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().italic_selected();
        },
        true);

    // superscript
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_EQUAL},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().superscript_selected();
        },
        true);

    // subscript
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_MINUS},
        [&]() {
            if (!currentDocument().is_selecting()) return;
            currentDocument().save_snapshot();
            currentDocument().subscript_selected();
        },
        true);

    /* */
    mKeybind.insert(
        {KEY_ESCAPE},
        [&]() {
            if (mMode == EditorMode::Insert) {
                currentDocument().turn_off_selecting();
            } else if (mMode == EditorMode::Search) {
                mMode = EditorMode::Insert;
            }
        },
        false);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_F}, [&]() { mMode = EditorMode::Search; },
        false);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_H},
        [&]() {
            bool valid_word = currentDocument().check_word_at_cursor();
            std::vector< nstring > suggestions =
                currentDocument().suggest_at_cursor();

            std::cout << "The word is "
                      << (valid_word ? "valid"
                                     : "not exist in normal English vocabulary")
                      << std::endl;

            for (std::size_t i = 0; i < std::min(11, (int)suggestions.size());
                 ++i) {
                std::cout << i << ": " << suggestions[i] << std::endl;
            }
        },
        false);

    /* Mode switching */
    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_I},
        [&]() {
            if (mMode == EditorMode::Insert) {
                mMode = EditorMode::Normal;
            } else if (mMode == EditorMode::Normal) {
                mMode = EditorMode::Insert;
            }
        },
        false);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_K},
        [&]() {
            if (mPage == EditorPage::Link) {
                mMode = EditorMode::Insert;
                mPage = EditorPage::None;
                return;
            }
            if (!currentDocument().is_selecting()) return;
            mMode = EditorMode::Normal;
            mPage = EditorPage::Link;
        },
        false);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_C},
        [&]() {
            if (mPage == EditorPage::Color) {
                mMode = EditorMode::Insert;
                mPage = EditorPage::None;

                currentColor = BLACK;
                currentBackgroundColor = Color{0, 0, 0, 0};
            } else {
                mMode = EditorMode::Normal;
                mPage = EditorPage::Color;

                currentColor = currentDocument().get_text_color();
                currentBackgroundColor =
                    currentDocument().get_background_color();
            }
        },
        false);
}

Editor::Editor() {}

Editor::~Editor() {
    delete fonts;
    // Close();
}