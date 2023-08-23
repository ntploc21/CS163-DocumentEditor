#include "keybind/keybind.hpp"

#include <iostream>

int modify_key(int key) {
    switch (key) {
        case KEY_LEFT_SHIFT:
            [[fallthrough]];
        case KEY_RIGHT_SHIFT:
            return KEY_LEFT_SHIFT;

        case KEY_LEFT_CONTROL:
            [[fallthrough]];
        case KEY_RIGHT_CONTROL:
            return KEY_LEFT_CONTROL;

        case KEY_LEFT_ALT:
            [[fallthrough]];
        case KEY_RIGHT_ALT:
            return KEY_LEFT_ALT;

        default:
            return key;
    }
}

Keybind::~Keybind() { delete mRoot; }

void Keybind::insert(const std::vector< int >& keys,
                     const std::function< void() >& func, bool editable) {
    Node* current = mRoot;

    int key = keys.back();

    for (std::size_t i = 0; i < keys.size() - 1; ++i) {
        int key = keys[i];
        if (current->child(key) == nullptr) {
            current->child(key) = new Node;
            current->child(key)->set_parent(current);
        }
        current = current->child(key);
    }
    current->child(key) = new keybind::FunctionNode(func, editable);
}

void Keybind::process(bool editable) {
    int key = GetKeyPressed();

    if (key == KEY_NULL) {
        if (mPressedKeys.size()) {
            std::vector< int > newPressedKeys;

            for (int prev_key : mPressedKeys) {
                if (IsKeyDown(prev_key)) {
                    newPressedKeys.push_back(prev_key);
                }
            }

            mPressedKeys = newPressedKeys;
        }
        return;
    }

    if ((KEY_APOSTROPHE <= key && key <= KEY_GRAVE) || key == KEY_ENTER) {
        executeKeybinds(key, editable);
    } else if (KEY_LEFT_SHIFT <= key && key <= KEY_RIGHT_SUPER) {
        bool pressed = false;
        for (int prev_key : mPressedKeys) {
            if (prev_key == key) {
                pressed = true;
                break;
            }
        }
        if (!pressed) {
            mPressedKeys.push_back(key);
        }
        return;
    } else {
        return;
    }
}

void Keybind::executeKeybinds(int key, bool editable) const {
    Node* current = mRoot;
    std::vector< int > keybinds = mPressedKeys;
    keybinds.push_back(key);

    for (int key : keybinds) {
        key = modify_key(key);
        if (current->child(key) == nullptr) {
            return;
        }
        current = current->child(key);
    }
    if (current->is_callable()) {
        current->call(editable);
    }
}