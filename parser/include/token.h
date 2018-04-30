#ifndef PARSER_TOKEN_H
#define PARSER_TOKEN_H

#include <string>
#include <cmath>

class Token {
protected:
    int line;
    int begin;
    int fin;
public:
    Token(int line, int begin, int fin) : line(line), begin(begin), fin(fin) {}

    virtual std::string get_type() {
        return "Token";
    }

    virtual int get_line() {
        return line;
    }

    virtual int get_start_pos() {
        return begin;
    }

    virtual int get_end_pos() {
        return fin;
    }

    virtual void print() {
        std::cout << "Token(" << line << ", " << begin << ", " << fin << ")";
    }
};


class Ident : public Token {
private:
    std::string name;
public:
    Ident(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin),
                                                               name(lexeme) {}

    virtual std::string get_type() {
        return "Ident";
    }

    std::string get_ident() {
        return name;
    }

    virtual void print() {
        std::cout << "IDENT(\"" << name << "\", " << line << ", " << begin << ", " << fin << ")";
    }
};

class KeyWord : public Token {
private:
    std::string word;

public:
    KeyWord(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin),
                                                                word(lexeme) {}

    virtual std::string get_type() {
        return "KeyWord";
    }

    std::string get_word() {
        return word;
    }

    virtual void print() {
        std::cout << "KW_" << word << "(" << line << ", " << begin << ", " << fin << ")";
    }
};

class Num : public Token {
private:
    double value;

public:
    Num(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin) {
        try {
            if (lexeme[0] == '.') {
                value = std::stod("0" + lexeme);
            } else {
                value = std::stod(lexeme);
            }
        } catch (const std::out_of_range& oor) {
            value = INFINITY;
        }
    }

    virtual std::string get_type() {
        return "Num";
    }

    double get_value() {
        return value;
    }

    virtual void print() {
        std::cout << "NUM(" << value << ", " << line << ", " << begin << ", " << fin << ")";
    }
};


class Bool : public Token {
private:
    std::string lit;

public:
    Bool(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin),
                                                             lit(lexeme) {}

    virtual std::string get_type() {
        return "Bool";
    }

    std::string get_lit() {
        return lit;
    }

    virtual void print() {
        std::cout << "BOOL(" << lit << ", " << line << ", " << begin << ", " << fin << ")";
    }
};

class Operator : public Token {
private:
    std::string op;

public:
    Operator(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin),
                                                                 op(lexeme) {}

    virtual std::string get_type() {
        return "Operator";
    }

    std::string get_op() {
        return op;
    }

    virtual void print() {
        std::cout << "OPERATOR(\"" << op << "\", " << line << ", " << begin << ", " << fin << ")";
    }
};

class Separator : public Token {
private:
    std::string sep;

public:
    Separator(int line, int begin, int fin, std::string& lexeme) : Token(line, begin, fin),
                                                                  sep(lexeme) {}

    virtual std::string get_type() {
        return "Separator";
    }

    std::string get_sep() {
        return sep;
    }

    virtual void print() {
        std::cout << "SEPARATOR(\"" << sep << "\", " << line << ", " << begin << ", " << fin << ")";
    }
};

#endif //PARSER_TOKEN_H
