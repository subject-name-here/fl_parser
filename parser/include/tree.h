#ifndef PARSER_TREE_H
#define PARSER_TREE_H

#include "token.h"
#include <vector>
#include <memory>

struct Node {
    std::shared_ptr<Token> begin;
    std::shared_ptr<Token> fin;
    std::vector<std::shared_ptr<Node>> sons;
    std::string name;
    bool is_atom = false;

    Node(std::shared_ptr<Token> begin, std::shared_ptr<Token> fin, std::string name)
            : begin(begin), fin(fin), name(name) {};

    Node(std::shared_ptr<Token> t, std::string name)
            : begin(t), fin(t), name(name) {};

    Node(std::string name) : name(name) {};

    Node(Node* node) {
        begin = node->begin;
        fin = node->fin;
        name = node->name;
        sons = node->sons;
        is_atom = node->is_atom;
    };

    int print(std::ofstream& os, int id) {
        if (is_atom || !sons.empty()) {
            std::string b = std::to_string(begin->get_line()) + ":" + std::to_string(begin->get_start_pos());
            std::string e;
            if (is_atom) {
                e = std::to_string(begin->get_line()) + ":" + std::to_string(begin->get_end_pos());
            } else {
                e = std::to_string(fin->get_line()) + ":" + std::to_string(fin->get_end_pos());
            }
            os << "S" << id << " [shape=record, label=\"{" << name << "|{" << b << "|" << e << "}}\"];\n";

            int m = id + 1;
            for (auto& son : sons) {
                int last_m = m;
                m = son->print(os, m);
                os << "S" << id << "->" << "S" << last_m << ";\n";
            }

            return m;
        } else {
            os << "S" << id << " [shape=record, label=\"{" << name << "}\"];\n";
            return id + 1;
        }
    }
};


#endif //PARSER_TREE_H
