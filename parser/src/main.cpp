#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <tree.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"

void test_all() {

}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: wrong number of arguments.\n";
        return 1;
    }

    std::string s = argv[1];
    if (s == "test") {
        test_all();
    } else {
        std::ifstream is;
        std::string expr;

        is.open(s);
        getline(is, expr);

        if (is.fail()) {
            std::cerr << "File not found or file is empty.\n";
            is.close();
            return 1;
        }

        try {
            auto tokens = Lexer::tokenify(expr);
            Tree t = Parser::parse(tokens);
            t.print();
            std::cout << std::endl;
        } catch (std::string& e) {
            std::cerr << e;
            is.close();
            return 1;
        }

    }


    return 0;
}