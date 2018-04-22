#ifndef PARSER_TREE_H
#define PARSER_TREE_H

#include "token.h"
/*
struct Tree {
    Tree() {}

    Tree(const Tree& t) {
        node = t.node;
        left = t.left;
        right = t.right;
    }

    std::shared_ptr<Token> node;
    std::shared_ptr<Tree> left;
    std::shared_ptr<Tree> right;

    void print() {
        if (left && right) {
            std::cout << "[";
        }

        std::string node_type = node.get()->get_type();
        if (node_type == "NumToken") {
            NumToken tok = *dynamic_cast<NumToken*>(node.get());
            std::cout << tok.get_num();
        } else if (node_type == "PowerToken") {
            std::cout << "^";
        } else if (node_type == "PlusMinusToken") {
            PlusMinusToken tok = *dynamic_cast<PlusMinusToken*>(node.get());
            std::cout << tok.get_symb();
        } else if (node_type == "MulDivToken") {
            MulDivToken tok = *dynamic_cast<MulDivToken*>(node.get());
            std::cout << tok.get_symb();
        } else {
            std::cout << "ERR";
        }

        if (left && right) {
            std::cout << ", ";
            if (left) {
                left.get()->print();
            }
            std::cout << ", ";
            if (right) {
                right.get()->print();
            }
            std::cout << "]";
        }

    }
};
*/
#endif //PARSER_TREE_H
