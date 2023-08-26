#include "editor.hpp"

#include <string.h>

#include <iostream>
#include <locale>
#include <set>
#include <vector>

#include "clip.h"
#include "constants.hpp"
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

    mKeybind.process(true);

    if (WindowShouldClose()) closed = true;
}

void Editor::Render() {
    BeginDrawing();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Color{249, 251, 253, 255});

    DrawEditor();

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
                                  charSize.x + 1, line_height, LIGHTGRAY);
                    //   Color{109, 158, 235, 255});
                }
            }
        }

        // draw text
        for (std::size_t i = line_start; i < next_line_start; ++i) {
            Font charFont = getFont(content[i]);
            std::size_t charFontSize = content[i].getFontSize();
            Color textColor = content[i].getColor();

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
}

Editor::Editor() {}

Editor::~Editor() {
    delete fonts;
    // Close();
}