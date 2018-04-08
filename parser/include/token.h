#ifndef PARSER_TOKEN_H
#define PARSER_TOKEN_H

class Token {
public:
    virtual std::string get_type() {
        return "Token";
    }

    virtual int get_pos() {
        return pos;
    }

protected:
    int pos = -15;
};

class NumToken : public Token {
public:
    NumToken(int num, int pos) : num(num), pos(pos) {}

    virtual std::string get_type() {
        return "NumToken";
    }

    int get_num() {
        return num;
    }

    virtual int get_pos() {
        return pos;
    }

private:
    int num;
    int pos;
};

class OpenBracketToken : public Token {
public:
    explicit OpenBracketToken(int pos) : pos(pos) {};

    virtual std::string get_type() {
        return "OpenBracketToken";
    }

    virtual int get_pos() {
        return pos;
    }

private:
    int pos;
};

class CloseBracketToken : public Token {
public:
    explicit CloseBracketToken(int pos) : pos(pos) {};

    virtual std::string get_type() {
        return "CloseBracketToken";
    }

    virtual int get_pos() {
        return pos;
    }

private:
    int pos;
};

class PowerToken : public Token {
public:
    explicit PowerToken(int pos) : pos(pos) {};

    virtual std::string get_type() {
        return "PowerToken";
    }

    virtual int get_pos() {
        return pos;
    }


private:
    int pos;
};

class MulDivToken : public Token {
public:
    MulDivToken(char symb, int pos) : symb(symb), pos(pos) {}

    virtual std::string get_type() {
        return "MulDivToken";
    }

    char get_symb() {
        return symb;
    }

    virtual int get_pos() {
        return pos;
    }

private:
    char symb;
    int pos;
};

class PlusMinusToken : public Token {
public:
    PlusMinusToken(char symb, int pos) : symb(symb), pos(pos) {}

    virtual std::string get_type() {
        return "PlusMinusToken";
    }

    char get_symb() {
        return symb;
    }

    virtual int get_pos() {
        return pos;
    }

private:
    char symb;
    int pos;
};

#endif //PARSER_TOKEN_H
