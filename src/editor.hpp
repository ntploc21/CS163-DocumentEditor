#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "FontFactory.hpp"
#include "document.hpp"
#include "keybind/keybind.hpp"
#include "raylib.h"

enum class EditorMode { Normal, Insert };

/**
 * @brief The application class that represents the application.
 * @details The application class is used to represent the application. It
 * contains the main loop of the application.
 */
class Editor {
public:
    /**
     * @brief Construct a new Application object.
     */
    Editor();

    /**
     * @brief Destroy the Application object.
     */
    ~Editor();

    /**
     * @brief Run the application.
     * @details This function runs the application.
     */
    void Run();

    /**
     * @brief Close the application.
     * @details This function closes the application.
     */
    void Close();

    void Init();

    /**
     * @brief Check if the window is closed.
     * @details This function checks if the window is closed.
     * @return True if the window is closed.
     */
    bool WindowClosed();

private:
    /**
     * @brief Draw/render the application.
     * @details This function draws the application.
     */
    void Render();

    /**
     * @brief Load the resources of the application.
     * @details This function loads the resources of the application.
     * @see TextureHolder::Load
     * @see FontHolder::Load
     */
    void LoadResources();

    /**
     * @brief Update the application.
     * @details This function updates the application.
     * @param dt The delta time.
     */
    void Update(float dt);

    void PrepareKeybinds();

    void DrawEditor();
    void DrawEditorText();

    void NormalMode();

    void InsertMode();

    Document& currentDocument();

    const Document& currentDocument() const;

private:
    bool closed = false;
    Keybind mKeybind{};
    Document mDocument{};

    FontFactory* fonts{new FontFactory};

    EditorMode mMode{EditorMode::Insert};
};

#endif  // EDITOR_HPP