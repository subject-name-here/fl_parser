#ifndef PARSER_LEXER_H
#define PARSER_LEXER_H

#include <vector>
#include <string>
#include "token.h"
#include "token_stream.h"

class Lexer {
public:
    static TokenStream tokenify(std::string& s) {
        TokenStream tokens;
        int line = 0;
        int symb = 0;
        int cnt = 0;

        while (cnt < s.length()) {
            int d;

            if (s[cnt] == '_') {
                d = find_ident(s, cnt);
                std::string lexeme = s.substr(cnt, d);
                tokens.add(new Ident(line, symb, symb + d - 1, lexeme));
            } else if (s[cnt] >= 'a' && s[cnt] <= 'z') {
                d = find_bool(s, cnt);
                if (d == -1)  {
                    d = find_keyword(s, cnt);
                    if (d == -1) {
                        d = find_ident(s, cnt);
                        std::string lexeme = s.substr(cnt, d);
                        tokens.add(new Ident(line, symb, symb + d - 1, lexeme));
                    } else {
                        std::string lexeme = s.substr(cnt, d);
                        tokens.add(new KeyWord(line, symb, symb + d - 1, lexeme));
                    }
                } else {
                    std::string lexeme = s.substr(cnt, d);
                    tokens.add(new Bool(line, symb, symb + d - 1, lexeme));
                }
            } else if (s[cnt] == '.' || s[cnt] >= '0' && s[cnt] <= '9') {
                d = find_num(s, cnt);
                if (d == -1) {
                    std::string e = "Exception: unrecognized numeric literal at line ";
                    e.append(std::to_string(line));
                    e.append(", position ");
                    e.append(std::to_string(symb));
                    e.append(".");
                    throw e;
                }

                std::string lexeme = s.substr(cnt, d);
                tokens.add(new Num(line, symb, symb + d - 1, lexeme));

            } else if (s[cnt] == 32 || s[cnt] == 9 || s[cnt] == 12) {
                d = 1;
            } else if (s[cnt] == 10 || s[cnt] == 13) {
                d = 1;
                if (cnt + 1 < s.length() && s[cnt] == 10 && s[cnt + 1] == 13) {
                    d++;
                }
                line++;
                symb = -d;
            } else {
                d = find_separator(s, cnt);
                if (d != -1) {
                    std::string lexeme = s.substr(cnt, d);
                    tokens.add(new Separator(line, symb, symb + d - 1, lexeme));
                } else {
                    d = find_operator(s, cnt);
                    if (d == -1) {
                        std::string e = "Exception: unknown lexeme at line ";
                        e.append(std::to_string(line));
                        e.append(", position ");
                        e.append(std::to_string(symb));
                        e.append(".");
                        throw e;
                    }
                    std::string lexeme = s.substr(cnt, d);
                    tokens.add(new Operator(line, symb, symb + d - 1, lexeme));
                }
            }
            cnt += d;
            symb += d;
        }

        return tokens;
    }

    static int find_ident(std::string& s, int pos) {
        if (s[pos] != '_' && (s[pos] < 'a' || s[pos] > 'z')) {
            return -1;
        }
        int l = 1;
        while (pos + l < s.length() && (s[pos + l] == '_' ||
                                        s[pos + l] >= '0' && s[pos + l] <= '9' ||
                                        s[pos + l] >= 'a' && s[pos + l] <= 'z')) {
            l++;
        }

        return l;
    }

    static int find_keyword(std::string& s, int pos) {
        if (pos + 2 <= s.length()) {
            std::string w =  s.substr(pos, 2);
            if (w == "if" || w == "do") {
                return 2;
            }
        }

        if (pos + 4 <= s.length()) {
            std::string w =  s.substr(pos, 4);
            if (w == "then" || w == "else" || w == "read") {
                return 4;
            }
        }

        if (pos + 5 <= s.length()) {
            std::string w =  s.substr(pos, 5);
            if (w == "while" || w == "write") {
                return 5;
            }
        }

        return -1;
    }

    static int find_digits(std::string& s, int pos) {
        int l = 0;
        while (pos + l < s.length() && s[pos + l] >= '0' && s[pos + l] <= '9') {
            l++;
        }

        return l;
    }

    static int find_exp(std::string& s, int pos) {
        int l = 0;

        if (s[pos] == 'e' || s[pos] == 'E') {
            l++;
            if (s[pos + l] == '+' || s[pos + l] == '-') {
                l++;
            }

            int d = find_digits(s, pos + l);
            if (d > 0) {
                return l + d;
            }
        }

        return 0;
    }

    static int find_num(std::string& s, int pos) {
        if (s[pos] == '.') {
            int l = 1 + find_digits(s, pos + 1);
            if (l > 0) {
                l += find_exp(s, pos + l);
                return l;
            }
        } else {
            int l = find_digits(s, pos);
            if (l > 0) {
                if (s[pos + l] == '.') {
                    l++;
                    l += find_digits(s, pos + l);
                    l += find_exp(s, pos + l);
                } else {
                    l += find_exp(s, pos + l);
                }
                return l;
            }
        }

        return -1;
    }

    static int find_bool(std::string& s, int pos) {
        if (pos + 4 <= s.length() && s.substr(pos, 4) == "true") {
            return 4;
        }

        if (pos + 5 <= s.length() && s.substr(pos, 5) == "false") {
            return 5;
        }

        return -1;
    }

    static int find_operator(std::string& s, int pos) {
        if (s[pos] == '+' || s[pos] == '-' || s[pos] == '*' || s[pos] == '/' || s[pos] == '%') {
            return 1;
        }

        if (s[pos] == '>' || s[pos] == '<') {
            if (pos + 1 < s.length() && s[pos + 1] == '=') {
                return 2;
            } else {
                return 1;
            }
        }

        if (pos + 1 < s.length() &&
            (s[pos] == '=' && s[pos + 1] == '=' ||
             s[pos] == '!' && s[pos + 1] == '=' ||
             s[pos] == '&' && s[pos + 1] == '&' ||
             s[pos] == '|' && s[pos + 1] == '|')) {
            return 2;
        }

        return -1;
    }

    static int find_separator(std::string& s, int pos) {
        if (s[pos] == '(' || s[pos] == ')' || s[pos] == ';' || s[pos] == ',') {
            return 1;
        }
        return -1;
    }
};

#endif //PARSER_LEXER_H
