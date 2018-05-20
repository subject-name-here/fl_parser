#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>

#include "lexer.h"
#include "token.h"
#include "token_stream.h"
#include "parser.h"

std::stringstream buffer;
char result[1024];

void print(Node *pNode, std::string basic_string);

bool test1() {
    std::string t1 = "ident1 ident2 __ ident4 xxx_coolident666_xxx";
    try {
        TokenStream ts = Lexer::tokenify(t1);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "IDENT(\"ident1\", 0, 0, 5), "
                         "IDENT(\"ident2\", 0, 7, 12), "
                         "IDENT(\"__\", 0, 14, 15), "
                         "IDENT(\"ident4\", 0, 17, 22), "
                         "IDENT(\"xxx_coolident666_xxx\", 0, 24, 43)";


    return expected.compare(result) == 0;
}

bool test2() {
    std::string t2 = "while read if do then write else do";
    try {
        TokenStream ts = Lexer::tokenify(t2);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "KW_while(0, 0, 4), "
                           "KW_read(0, 6, 9), "
                           "KW_if(0, 11, 12), "
                           "KW_do(0, 14, 15), "
                           "KW_then(0, 17, 20), "
                           "KW_write(0, 22, 26), "
                           "KW_else(0, 28, 31), "
                           "KW_do(0, 33, 34)";


    return expected.compare(result) == 0;
}

bool test3() {
    std::string t3 = "42 23. 1.6 .15 8.e4";
    try {
        TokenStream ts = Lexer::tokenify(t3);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "NUM(42, 0, 0, 1), "
                           "NUM(23, 0, 3, 5), "
                           "NUM(1.6, 0, 7, 9), "
                           "NUM(0.15, 0, 11, 13), "
                           "NUM(80000, 0, 15, 18)";

    return expected.compare(result) == 0;
}

bool test4() {
    std::string t4 = "(true, false); (false), (true);";
    try {
        TokenStream ts = Lexer::tokenify(t4);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "SEPARATOR(\"(\", 0, 0, 0), "
                           "BOOL(true, 0, 1, 4), "
                           "SEPARATOR(\",\", 0, 5, 5), "
                           "BOOL(false, 0, 7, 11), "
                           "SEPARATOR(\")\", 0, 12, 12), "
                           "SEPARATOR(\";\", 0, 13, 13), "
                           "SEPARATOR(\"(\", 0, 15, 15), "
                           "BOOL(false, 0, 16, 20), "
                           "SEPARATOR(\")\", 0, 21, 21), "
                           "SEPARATOR(\",\", 0, 22, 22), "
                           "SEPARATOR(\"(\", 0, 24, 24), "
                           "BOOL(true, 0, 25, 28), "
                           "SEPARATOR(\")\", 0, 29, 29), "
                           "SEPARATOR(\";\", 0, 30, 30)";

    return expected.compare(result) == 0;
}

bool test5() {
    std::string t5 = "+-*/ >===<= && ++ ||";
    try {
        TokenStream ts = Lexer::tokenify(t5);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "OPERATOR(\"+\", 0, 0, 0), "
                           "OPERATOR(\"-\", 0, 1, 1), "
                           "OPERATOR(\"*\", 0, 2, 2), "
                           "OPERATOR(\"/\", 0, 3, 3), "
                           "OPERATOR(\">=\", 0, 5, 6), "
                           "OPERATOR(\"==\", 0, 7, 8), "
                           "OPERATOR(\"<=\", 0, 9, 10), "
                           "OPERATOR(\"&&\", 0, 12, 13), "
                           "OPERATOR(\"+\", 0, 15, 15), "
                           "OPERATOR(\"+\", 0, 16, 16), "
                           "OPERATOR(\"||\", 0, 18, 19)";

    return expected.compare(result) == 0;
}

bool test6() {
    std::string t6 = "if (a) then b+ \n else true || b;";
    try {
        TokenStream ts = Lexer::tokenify(t6);
        ts.print();
    } catch (std::string e) {
        std::cerr << e << std::endl;
        return false;
    }
    buffer.getline(result, 1024);
    std::string expected = "KW_if(0, 0, 1), "
                           "SEPARATOR(\"(\", 0, 3, 3), "
                           "IDENT(\"a\", 0, 4, 4), "
                           "SEPARATOR(\")\", 0, 5, 5), "
                           "KW_then(0, 7, 10), "
                           "IDENT(\"b\", 0, 12, 12), "
                           "OPERATOR(\"+\", 0, 13, 13), "
                           "KW_else(1, 1, 4), "
                           "BOOL(true, 1, 6, 9), "
                           "OPERATOR(\"||\", 1, 11, 12), "
                           "IDENT(\"b\", 1, 14, 14), "
                           "SEPARATOR(\";\", 1, 15, 15)";

    return expected.compare(result) == 0;
}

bool test_err() {
    std::string t = "a ~= b";
    try {
        TokenStream ts = Lexer::tokenify(t);
        return false;
    } catch (std::string e) {
        //std::cerr << e << std::endl;
        return true;
    }
}


void test_all() {
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    bool all_passed = true;

    if (!test1()) {
        all_passed = false;
        std::cerr << "Failed test 1.\n";
    }

    if (!test2()) {
        all_passed = false;
        std::cerr << "Failed test 2.\n";
    }

    if (!test3()) {
        all_passed = false;
        std::cerr << "Failed test 3.\n";
    }

    if (!test4()) {
        all_passed = false;
        std::cerr << "Failed test 4.\n";
    }

    if (!test5()) {
        all_passed = false;
        std::cerr << "Failed test 5.\n";
    }

    if (!test6()) {
        all_passed = false;
        std::cerr << "Failed test 6.\n";
    }

    if (!test_err()) {
        all_passed = false;
        std::cerr << "Failed test err.\n";
    }


    std::cout.rdbuf(old);
    //std::cout << result << std::endl;

    if (all_passed) {
        std::cout << "All tests passed." << std::endl;
    }
}


void print(Node *n, std::string filename) {
    std::ofstream os;
    os.open(filename + ".gv");

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
            //tokens.print();
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

