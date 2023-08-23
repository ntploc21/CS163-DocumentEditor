#ifndef KEYBIND_KEYBIND_HPP
#define KEYBIND_KEYBIND_HPP

#include <functional>
#include <vector>

#include "keybind/node.hpp"
#include "raylib.h"

class Keybind {
public:
    ~Keybind();

    void process(bool editable);

    void insert(const std::vector< int >& keys,
                const std::function< void() >& func, bool editable);

    void executeKeybinds(int key, bool editable) const;

private:
    using Node = keybind::Node;

    Node* mRoot{new Node};

    std::vector< int > mPressedKeys;
};

#endif  // KEYBIND_KEYBIND_HPP