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
            if (s[cnt] == 32 || s[cnt] == 9 || s[cnt] == 12) {
                // It's space.
                cnt++;
                symb++;
                continue;
            } else if (s[cnt] == 10 || s[cnt] == 13) {
                // It's breakline.
                cnt++;
                if (cnt + 1 < s.length() && s[cnt] == 10 && s[cnt + 1] == 13) {
                    cnt++;
                }

                line++;
                symb = 0;
                continue;
            }

            int d1 = find_keyword(s, cnt);
            int d2 = find_bool(s, cnt);
            int d3 = find_ident(s, cnt);
            int d4 = find_num(s, cnt);
            int d5 = find_operator(s, cnt);
            int d6 = find_separator(s, cnt);
            int d7 = find_unknown(s, cnt);

            if (d1 != -1) {
                std::string lexeme = s.substr(cnt, d1);
                tokens.add(new Keyword(line, symb, symb + d1 - 1, lexeme));
                symb += d1;
                cnt += d1;
            } else if (d2 != -1) {
                std::string lexeme = s.substr(cnt, d2);
                tokens.add(new Bool(line, symb, symb + d2 - 1, lexeme));
                symb += d2;
                cnt += d2;
            } else if (d3 != -1) {
                std::string lexeme = s.substr(cnt, d3);
                tokens.add(new Ident(line, symb, symb + d3 - 1, lexeme));
                symb += d3;
                cnt += d3;
            } else if (d4 != -1) {
                std::string lexeme = s.substr(cnt, d4);
                tokens.add(new Num(line, symb, symb + d4 - 1, lexeme));
                symb += d4;
                cnt += d4;
            } else if (d5 != -1) {
                std::string lexeme = s.substr(cnt, d5);
                tokens.add(new Operator(line, symb, symb + d5 - 1, lexeme));
                symb += d5;
                cnt += d5;
            } else if (d6 != -1) {
                std::string lexeme = s.substr(cnt, d6);
                tokens.add(new Separator(line, symb, symb + d6 - 1, lexeme));
                symb += d6;
                cnt += d6;
            } else {
                std::string lexeme = s.substr(cnt, d7);
                tokens.add(new Unknown(line, symb, symb + d7 - 1, lexeme));
                symb += d7;
                cnt += d7;
            }
        }

        return tokens;
    }

    static int find_ident(std::string& s, int pos) {
        // Let's take all than can be taken.
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
        if (pos + 6 <= s.length()) {
            std::string w =  s.substr(pos, 6);
            if (w == "return" || w == "assert" || w == "if_not") {
                return 6;
            }
        }

        if (pos + 2 <= s.length()) {
            std::string w =  s.substr(pos, 2);
            if (w == "if" || w == "do" || w == "ex") {
                return 2;
            }
        }

        if (pos + 3 <= s.length()) {
            std::string w =  s.substr(pos, 3);
            if (w == "end") {
                return 3;
            }
        }

        if (pos + 4 <= s.length()) {
            std::string w =  s.substr(pos, 4);
            if (w == "then" || w == "else" || w == "read" || w == "pass" || w == "fall") {
                return 4;
            }
        }

        if (pos + 5 <= s.length()) {
            std::string w =  s.substr(pos, 5);
            if (w == "while" || w == "write" || w == "begin") {
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

        // Sugar!
        if (pos + 4 <= s.length() && s.substr(pos, 4) == "fail") {
            return 4;
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

        if (pos + 1 < s.length() && s[pos] == ':' && s[pos + 1] == '=') {
            return 2;
        }

        return -1;
    }

    static bool not_good_symbol(char i) {
        return !(i == '(' || i == ')' || i == ';' || i == ',' || i == 9 || i == 10 || i == 12 || i == 13 || i == 32);
    }

    static int find_unknown(std::string& s, int pos) {
        int l = 0;
        while (pos + l < s.length() && not_good_symbol(s[pos + l])) {
            l++;
        }

        return l;
    }
};

#endif //PARSER_LEXER_H
