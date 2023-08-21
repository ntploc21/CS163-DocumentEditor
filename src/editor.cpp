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
    LoadResources();
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

    if (WindowShouldClose()) closed = true;
}

void Editor::Render() {
    BeginDrawing();

    EndDrawing();
}

std::vector< int > s;
// std::string tmp;
Rope tmp;

void Editor::Update([[maybe_unused]] float dt) {
    int key = GetCharPressed();

    if (key) {
        if (key >= 128) s.pop_back();
        s.push_back(key);
        if (key >= 128) tmp = tmp.erase(tmp.length() - 1, 1);
        tmp = tmp.append(nstring(nchar(key)));
    }

    nstring tmpS;
    if (s.size()) {
        for (std::size_t i = 0; i < s.size(); i++) {
            tmpS += nchar(s[i]);
        }
    }

    DrawTextEx(font, tmpS.c_str(), {0, 100}, 72, 0, WHITE);

    // if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V) &&
    //     clip::has(clip::text_format())) {
    //     tmp.clear();
    //     clip::get_text(tmp);
    // }
    DrawTextEx(font, tmp.to_string().c_str(), {0, 0}, 72, 0, WHITE);
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
    font = LoadFontEx("assets/fonts/SVN-Times New Roman Bold.ttf", 36,
                      codepointsNoDups, 256);

    // Set bilinear scale filter for better font scaling
    SetTextureFilter(font.texture, TEXTURE_FILTER_ANISOTROPIC_16X);

    // Free codepoints, atlas has already been generated
    free(codepointsNoDups);
}

Editor::Editor() {}

Editor::~Editor() {
    UnloadFont(font);
    // Close();
}