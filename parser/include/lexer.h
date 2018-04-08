#ifndef PARSER_LEXER_H
#define PARSER_LEXER_H

#include <vector>
#include "token.h"

class Lexer {
public:
    static std::vector<std::shared_ptr<Token>> tokenify(std::string expr) {
        std::vector<std::shared_ptr<Token>> tokens;

        int num = 0;
        bool is_num = false;
        for (int i = 0; i < expr.length(); i++) {
            char c = expr[i];

            if (c >= '0' && c <= '9') {
                is_num = true;
                num = num * 10 + (c - '0');
            } else {
                if (is_num) {
                    tokens.push_back(std::make_shared<NumToken>(num, i - 1));
                    is_num = false;
                    num = 0;
                }

                if (c == '(') {
                    tokens.push_back(std::make_shared<OpenBracketToken>(i));
                } else if (c == ')') {
                    tokens.push_back(std::make_shared<CloseBracketToken>(i));
                } else if (c == '^') {
                    tokens.push_back(std::make_shared<PowerToken>(i));
                } else if (c == '*' || c == '/') {
                    tokens.push_back(std::make_shared<MulDivToken>(c, i));
                } else if (c == '+' || c == '-') {
                    tokens.push_back(std::make_shared<PlusMinusToken>(c, i));
                } else if (c != ' ') {
                    std::string s = "Fail: unknown symbol ";
                    s.push_back(c);
                    s.append(" on position ");
                    s.append(std::to_string(i));
                    throw s;
                }
            }
        }

        if (is_num) {
            tokens.push_back(std::make_shared<NumToken>(num, expr.length() - 1));
        }

        return tokens;
    }
};

#endif //PARSER_LEXER_H
