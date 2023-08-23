#ifndef KEYBIND_NODE_HPP
#define KEYBIND_NODE_HPP

#include <array>
#include <functional>

#include "constants.hpp"

namespace keybind {

    class Node {
    public:
        void set_parent(Node* parent);
        Node*& parent();

        std::array< Node*, constants::keyboard::char_limit >& children();
        const std::array< Node*, constants::keyboard::char_limit >& children()
            const;
        Node*& child(char c);

        virtual void call(bool editable){};
        virtual bool is_callable() { return false; };

        virtual ~Node(){};

    private:
        Node* mParent{};
        std::array< Node*, constants::keyboard::char_limit > mChildren{};
    };

    class FunctionNode : public Node {
    public:
        FunctionNode(const std::function< void() >& func, bool editable);

        void call(bool editable) override;
        bool is_callable() override { return true; };

    private:
        std::function< void() > mFunc;

        bool mEditable;
    };
};  // namespace keybind

#endif  // KEYBIND_NODE_HPP