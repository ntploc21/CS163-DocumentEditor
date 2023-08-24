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

static int *CodepointRemoveDuplicates(int *codepoints, int codepointCount,
                                      int *codepointResultCount);

void Editor::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(constants::window::width, constants::window::height,
               constants::window::title.c_str());

    MaximizeWindow();

    LoadResources();

    PrepareKeybinds();
}

static int *CodepointRemoveDuplicates(int *codepoints, int codepointCount,
                                      int *codepointsResultCount) {
    int codepointsNoDupsCount = codepointCount;
    int *codepointsNoDups = (int *)calloc(codepointCount, sizeof(int));
    memcpy(codepointsNoDups, codepoints, codepointCount * sizeof(int));

    // Remove duplicates
    for (int i = 0; i < codepointsNoDupsCount; i++) {
        for (int j = i + 1; j < codepointsNoDupsCount; j++) {
            if (codepointsNoDups[i] == codepointsNoDups[j]) {
                for (int k = j; k < codepointsNoDupsCount; k++)
                    codepointsNoDups[k] = codepointsNoDups[k + 1];

                codepointsNoDupsCount--;
                j--;
            }
        }
    }

    // NOTE: The size of codepointsNoDups is the same as original array but
    // only required positions are filled (codepointsNoDupsCount)

    *codepointsResultCount = codepointsNoDupsCount;
    return codepointsNoDups;
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
    // NOTE: We are loading 256 codepoints based on first character in our
    static char *text =
        " !\"#$%&\'()*+,-./"
        "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~"
        "aAàÀảẢãÃáÁạẠăĂằẰẳẲẵẴắẮặẶâÂầẦẩẨẫẪấẤậẬbBcCdDđĐeEèÈẻẺẽẼéÉẹẸêÊềỀểỂễỄếẾệỆ"
        "fFgGhHiIìÌỉỈĩĨíÍịỊjJkKlLmMnNoOòÒỏỎõÕóÓọỌôÔồỒổỔỗỖốỐộỘơƠờỜởỞỡỠớỚợỢpPqQrR"
        "sStTuUùÙủỦũŨúÚụỤưƯừỪửỬữỮứỨựỰvVwWxXyYỳỲỷỶỹỸýÝỵỴzZ";

    int codepointCount = 0;
    int *codepoints = LoadCodepoints(text, &codepointCount);

    // Removed duplicate codepoints to generate smaller font atlas
    int codepointsNoDupsCount = 0;
    int *codepointsNoDups = CodepointRemoveDuplicates(
        codepoints, codepointCount, &codepointsNoDupsCount);
    UnloadCodepoints(codepoints);

    // Load font containing all the provided codepoint glyphs
    // A texture font atlas is automatically generated
    font =
        LoadFontEx("assets/fonts/SVN-Arial 3.ttf", 36, codepointsNoDups, 256);

    // Set bilinear scale filter for better font scaling
    SetTextureFilter(font.texture, TEXTURE_FILTER_ANISOTROPIC_16X);

    // Free codepoints, atlas has already been generated
    free(codepointsNoDups);
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
        {KEY_ENTER}, [&]() { mDocument.insert_at_cursor(nstring("\n")); },
        true);
}

void Editor::DrawEditor() {
    int documentWidth = constants::document::default_view_width;
    int documentHeight = constants::document::default_view_height;
    int margin_top = constants::document::margin_top;
    DrawRectangle(std::max(0, (GetScreenWidth() - documentWidth) / 2),
                  margin_top, documentWidth, documentHeight, WHITE);

    DrawTextEx(
        font, mDocument.rope().to_string().c_str(),
        {1.0f * std::max(0, (GetScreenWidth() - documentWidth) / 2) + 10, 10},
        36, 0, BLACK);
}

void Editor::NormalMode() {}

void Editor::InsertMode() {
    int key = GetCharPressed();

    if (key) {
        std::cout << key << std::endl;

        if (key >= 128) mDocument.erase_at_cursor();
        mDocument.insert_at_cursor(nstring(nchar(key)));
    }
}

Editor::Editor() {}

Editor::~Editor() {
    UnloadFont(font);
    // Close();
}