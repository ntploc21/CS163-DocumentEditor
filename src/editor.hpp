#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "DocumentFont.hpp"
#include "FontFactory.hpp"
#include "dictionary/dictionary.hpp"
#include "document/document.hpp"
#include "keybind/keybind.hpp"
#include "raylib.h"
#include "search/search.hpp"

enum class EditorMode { Normal, Insert, Search };

enum class EditorPage { None, Link, Color, Search, SpellCheck };

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

    void SearchMode();

    Document& currentDocument();

    const Document& currentDocument() const;

    Search& currentSearch();

    const Search& currentSearch() const;

    void DrawOutline();

    void DrawPage();

    void SetPage(EditorPage page);

private:
    void DrawLinkPage(float initX, float initY);
    void setLinkPage(std::string url);
    std::string currentURL = "";

    void DrawColorPage(float initX, float initY);
    Color currentColor = BLACK;
    Color currentBackgroundColor = Color{0, 0, 0, 0};

    void DrawSearchPage(float initX, float initY);
    bool searchPageFirstInput = true;

    void DrawSpellCheckPage(float initX, float initY);
    std::vector< nstring > suggestions{};

private:
    bool closed = false;
    Keybind mKeybind{};
    Document mDocument{};
    Search mSearch{};
    Dictionary* mDictionary{new Dictionary};

    FontFactory* fonts{new FontFactory};
    DocumentFont* mDocumentFont{new DocumentFont};

    EditorMode mMode{EditorMode::Insert};
    EditorPage mPage{EditorPage::None};
};

#endif  // EDITOR_HPP