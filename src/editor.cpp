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

// static int *CodepointRemoveDuplicates(int *codepoints, int codepointCount,
//                                       int *codepointResultCount);

void Editor::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(constants::window::width, constants::window::height,
               constants::window::title.c_str());

    MaximizeWindow();

    LoadResources();

    mDocument.set_font_factory(fonts);

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
    }
}

void Editor::LoadResources() {
    fonts->Load("Arial", "assets/fonts/SVN-Arial 3.ttf");
}

void Editor::PrepareKeybinds() {
    mKeybind.insert(
        {KEY_BACKSPACE}, [&]() { mDocument.erase_at_cursor(); }, true);

    mKeybind.insert(
        {KEY_DELETE}, [&]() { mDocument.erase_at_cursor(); }, true);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_Z}, [&]() { mDocument.undo(); }, true);

    mKeybind.insert(
        {KEY_LEFT_CONTROL, KEY_Y}, [&]() { mDocument.redo(); }, true);

    mKeybind.insert(
        {KEY_ENTER},
        [&]() {
            std::cout << "enter" << std::endl;
            mDocument.insert_at_cursor(nstring("\n"));
            // mDocument.cursor_move_next_line();
        },
        true);

    mKeybind.insert(
        {KEY_LEFT}, [&]() { mDocument.cursor_move_prev_char(); }, true);

    mKeybind.insert(
        {KEY_RIGHT}, [&]() { mDocument.cursor_move_next_char(); }, true);

    mKeybind.insert(
        {KEY_UP}, [&]() { mDocument.cursor_move_line(-1); }, true);

    mKeybind.insert(
        {KEY_DOWN}, [&]() { mDocument.cursor_move_line(1); }, true);
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
    int documentWidth = constants::document::default_view_width;
    int documentHeight = constants::document::default_view_height;
    int margin_top = constants::document::margin_top;
    // Vector2 char_size = utils::measure_text(" ", 20, 0);

    const auto &cursor = mDocument.cursor();
    const auto &content = mDocument.rope();

    for (std::size_t i = 0; i < mDocument.rope().length(); ++i) {
        Vector2 pos = mDocument.get_display_positions(i);

        DrawTextEx(
            fonts->Get("Arial"), mDocument.rope()[i].getChar(),
            {1.0f * std::max(0, (GetScreenWidth() - documentWidth) / 2) - 1 +
                 constants::document::padding_left + pos.x,
             1.0f * margin_top - 1 + constants::document::padding_top + pos.y},
            36, 0, BLACK);
    }

    // draw cursor block
    std::size_t cursor_pos = mDocument.rope().index_from_pos(
        mDocument.cursor().line, mDocument.cursor().column);
    Vector2 cursor_display_pos = mDocument.get_display_positions(cursor_pos);

    DrawRectangle(1.0f * std::max(0, (GetScreenWidth() - documentWidth) / 2) -
                      1 + constants::document::padding_left +
                      cursor_display_pos.x,
                  1.0f * margin_top - 1 + constants::document::padding_top +
                      cursor_display_pos.y,
                  1.5f, 36, ORANGE);
}

void Editor::NormalMode() {}

void Editor::InsertMode() {
    int key = GetCharPressed();
    if (!key) return;

    if (key == '\n') {
        mDocument.insert_at_cursor(nstring(nchar(key)));
        mDocument.cursor_move_line(1);

        return;
    }

    if (key >= 128) mDocument.erase_at_cursor();
    mDocument.insert_at_cursor(nstring(nchar(key)));
}

Editor::Editor() {}

Editor::~Editor() {
    delete fonts;
    // Close();
}