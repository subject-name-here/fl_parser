#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <vector>
#include "token.h"

class Parser {
    struct Result {
        Result(Tree t, int rest) : t(t), rest(rest) {};

        Tree t;
        int rest;
    };
public:
    static Tree parse(std::vector<std::shared_ptr<Token>>& tokens) {
        Result r = parsePlusMinus(tokens, 0);
        if (r.rest < tokens.size()) {
            std::string err = "Failed to parse: can't parse the whole expression, error at position ";
            err.append(std::to_string(tokens[r.rest].get()->get_pos()));
            err.append(".");
            throw err;
        }

        return r.t;
    }

    static Result parsePlusMinus(std::vector<std::shared_ptr<Token>>& tokens, int pos) {
        Result r = parseMulDiv(tokens, pos);
        Tree t = r.t;
        int pos_new = r.rest;

        while (pos_new < tokens.size()) {
            if (tokens[pos_new].get()->get_type() != "PlusMinusToken") {
                break;
            }

            Result r_new = parseMulDiv(tokens, pos_new + 1);
            Tree t_new;
            t_new.left = std::make_shared<Tree>(t);
            t_new.right = std::make_shared<Tree>(r_new.t);
            PlusMinusToken tok = *dynamic_cast<PlusMinusToken*>(tokens[pos_new].get());
            t_new.node = std::make_shared<PlusMinusToken>(tok.get_symb(), tok.get_pos());

            t = t_new;

            pos_new = r_new.rest;
        }

        return Result(t, pos_new);
    }

    static Result parseMulDiv(std::vector<std::shared_ptr<Token>>& tokens, int pos) {
        Result r = parsePower(tokens, pos);
        Tree t = r.t;
        int pos_new = r.rest;

        while (pos_new < tokens.size()) {
            if (tokens[pos_new].get()->get_type() != "MulDivToken") {
                break;
            }

            Result r_new = parsePower(tokens, pos_new + 1);
            Tree t_new;
            t_new.left = std::make_shared<Tree>(t);
            t_new.right = std::make_shared<Tree>(r_new.t);
            MulDivToken tok = *dynamic_cast<MulDivToken*>(tokens[pos_new].get());
            t_new.node = std::make_shared<MulDivToken>(tok.get_symb(), tok.get_pos());

            t = t_new;

            pos_new = r_new.rest;
        }

        return Result(t, pos_new);
    }

    static Result parsePower(std::vector<std::shared_ptr<Token>>& tokens, int pos) {
        Result r = parseBrackets(tokens, pos);
        Tree t = r.t;
        int pos_new = r.rest;

        if (pos_new < tokens.size() && tokens[pos_new].get()->get_type() == "PowerToken") {
            Result r2 = parsePower(tokens, pos_new + 1);

            Tree t_new;
            t_new.left = std::make_shared<Tree>(t);
            t_new.right = std::make_shared<Tree>(r2.t);
            t_new.node = std::make_shared<PowerToken>(tokens[pos_new].get()->get_pos());

            t = t_new;

            pos_new = r2.rest;
        }

        return Result(t, pos_new);
    }

    static Result parseBrackets(std::vector<std::shared_ptr<Token>>& tokens, int pos) {
        if (pos < tokens.size() && tokens[pos].get()->get_type() == "OpenBracketToken") {
            Result r = parsePlusMinus(tokens, pos + 1);
            if (r.rest >= tokens.size()) {
                std::string e = "Failed to parse: expected close bracket, but found expression end.";
                throw e;
            }

            if (tokens[r.rest].get()->get_type() != "CloseBracketToken") {
                std::string e = "Failed to parse: expected close bracket at position ";
                e.append(std::to_string(tokens[r.rest].get()->get_pos()));
                e.append(".");
                throw e;
            }

            return Result(r.t, r.rest + 1);
        }

        return parseNum(tokens, pos);
    }

    static Result parseNum(std::vector<std::shared_ptr<Token>>& tokens, int pos) {
        if (tokens.size() <= pos) {
            std::string e = "Failed to parse: expected number, but found expression end.";
            throw e;
        }

        if (tokens[pos].get()->get_type() != "NumToken") {
            std::string e = "Failed to parse: expected number at position ";
            e.append(std::to_string(tokens[pos].get()->get_pos()));
            e.append(".");
            throw e;
        }

        Tree t;
        NumToken tok = *dynamic_cast<NumToken*>(tokens[pos].get());
        t.node = std::make_shared<NumToken>(tok.get_num(), tok.get_pos());

        return Result(t, pos + 1);
    }


};

#endif //PARSER_PARSER_H
