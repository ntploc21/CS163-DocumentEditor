#include "editor.hpp"
#include "constants.hpp"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION

void Editor::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(constants::window::width, constants::window::height,
               constants::window::title.c_str());
    LoadResources();
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

void Editor::Update(float dt) {}

void Editor::LoadResources() {
}

Editor::Editor() {}

Editor::~Editor() {}