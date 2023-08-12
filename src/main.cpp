#include "Editor.hpp"

int main() {
    Editor app;
    app.Init();
    while (!app.WindowClosed()) app.Run();
    app.Close();
}