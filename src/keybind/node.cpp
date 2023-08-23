#include "keybind/node.hpp"

namespace keybind {

    void Node::set_parent(Node* parent) { mParent = parent; }

    Node*& Node::parent() { return mParent; }

    std::array< Node*, constants::keyboard::char_limit >& Node::children() {
        return mChildren;
    }

    const std::array< Node*, constants::keyboard::char_limit >& Node::children()
        const {
        return mChildren;
    }

    Node*& Node::child(char c) { return mChildren[c]; }

    FunctionNode::FunctionNode(const std::function< void() >& func,
                               bool editable)
        : mFunc(func), mEditable(editable) {}

    void FunctionNode::call(bool editable) {
        if (mEditable && !editable) return;
        std::invoke(mFunc);  // equivalent to mFunc()
    }

};  // namespace keybind