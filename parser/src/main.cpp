#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>

#include "lexer.h"
#include "token.h"
#include "token_stream.h"
#include "parser.h"

void test_all() {

}


void print(Node *n, std::string filename) {
    std::ofstream os;
    std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
    os.open(base_filename + ".gv");

    os << "digraph program {" << "\n";
        n->print(os, 0);

    os << "}";
    os.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: wrong number of arguments.\n";
        return 1;
    }

    std::string filename = argv[1];
    if (filename == "test") {
        test_all();
    } else {
        std::ifstream is;
        std::string str;

        is.open(filename);

        if (is.fail()) {
            std::cerr << "File not found.\n";
            is.close();
            return 1;
        }

        try {
            std::string code;

            char c;
            while (is.get(c)) {
                code.append(1, c);
            }

            TokenStream tokens = Lexer::tokenify(code);
            // tokens.print();
            Node* n = Parser::parse_program(tokens);

            print(n, filename);
        } catch (std::string& e) {
            std::cerr << e << "\n";
            is.close();
            return 1;
        }
        is.close();
    }

    return 0;
}

