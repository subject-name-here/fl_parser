#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <vector>
#include "token.h"
#include "tree.h"
#include "token_stream.h"

typedef struct Result {
    Result(Node t, int rest) : rest(rest) { node = std::make_shared<Node>(t); };

    std::shared_ptr<Node> node;
    int rest;
} Result;

class Parser {
public:
    static Node* parse_program(TokenStream& tokens) {
        Node* program = new Node(tokens.get(0), tokens.get(tokens.size() - 1), "Program");

        Result r = parse_functions(tokens, 0);
        int new_pos = r.rest;
        Result r2 = parse_body(tokens, new_pos);

        program->sons.emplace_back(r.node);
        program->sons.emplace_back(r2.node);
        if (r2.rest < tokens.size()) {
            Node unknown(tokens.get(r2.rest), tokens.get(tokens.size() - 1), "Unknown tail");
            program->sons.push_back(std::make_shared<Node>(unknown));
        }

        return program;
    }


    static Result parse_functions(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node funcs("Functions");
        funcs.begin = tokens.get(pos);
        funcs.fin = tokens.get(pos);

        while (new_pos < tokens.size() && tokens.get(new_pos)->get_type() == "Ident") {
            Result r = parse_function(tokens, new_pos);
            funcs.sons.emplace_back(r.node);
            new_pos = r.rest;
            if (new_pos < tokens.size() && tokens.get(new_pos)->get_type() == "Separator" &&
                (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ";") {
                funcs.fin = tokens.get(new_pos);
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        }

        if (new_pos == pos) {
            funcs.name = "Functions (empty)";
        }

        return Result(funcs, new_pos);
    }

    static Result parse_function(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node func("Function");
        func.begin = tokens.get(new_pos);

        Result r = parse_declaration(tokens, new_pos);
        func.sons.emplace_back(r.node);
        new_pos = r.rest;

        Result r2 = parse_body(tokens, new_pos);
        func.sons.emplace_back(r2.node);
        new_pos = r2.rest;

        func.fin = tokens.get(new_pos - 1);

        return Result(func, new_pos);
    }

    static Result parse_body(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node body("Body");

        if (tokens.get(new_pos)->get_type() == "Keyword") {
            Keyword token = *dynamic_cast<Keyword*>(tokens.get(new_pos).get());
            if (token.get_word() == "begin") {
                body.begin = tokens.get(new_pos);
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        } else {
            // ERROR!
            throw 1;
        }

        Result r = parse_statements(tokens, new_pos);
        body.sons.push_back(r.node);
        new_pos = r.rest;

        if (tokens.get(new_pos)->get_type() == "Keyword") {
            Keyword token = *dynamic_cast<Keyword*>(tokens.get(new_pos).get());
            if (token.get_word() == "end") {
                body.fin = tokens.get(new_pos);
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        } else {
            // ERROR!
            throw 1;
        }

        return Result(body, new_pos);
    }


    static Result parse_declaration(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node decl("Declaration");

        if (tokens.get(new_pos)->get_type() == "Ident") {
            decl.begin = tokens.get(new_pos);
            Ident token = *dynamic_cast<Ident*>(tokens.get(new_pos).get());
            Node name(tokens.get(new_pos), "Function name '" + token.get_ident() + "'");
            decl.sons.push_back(std::make_shared<Node>(name));
            decl.sons.back()->is_atom = true;
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Separator") {
            Separator token = *dynamic_cast<Separator*>(tokens.get(new_pos).get());
            if (token.get_sep() == "(") {
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        } else {
            // ERROR!
            throw 1;
        }

        Result r = parse_args(tokens, new_pos);
        decl.sons.push_back(r.node);
        new_pos = r.rest;

        if (tokens.get(new_pos)->get_type() == "Separator") {
            Separator token = *dynamic_cast<Separator*>(tokens.get(new_pos).get());
            if (token.get_sep() == ")") {
                decl.fin = tokens.get(new_pos);
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        } else {
            // ERROR!
            throw 1;
        }

        return Result(decl, new_pos);
    }


    static Result parse_args(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node args("Args");

        if (tokens.get(new_pos)->get_type() == "Ident") {
            args.begin = tokens.get(new_pos);
            Ident token = *dynamic_cast<Ident*>(tokens.get(new_pos).get());
            Node arg(tokens.get(new_pos), "Function argument '" + token.get_ident()  + "'");
            args.sons.push_back(std::make_shared<Node>(arg));
            args.sons.back()->is_atom = true;
            new_pos++;

            while (tokens.get(new_pos)->get_type() == "Separator")  {
                Separator token2 = *dynamic_cast<Separator*>(tokens.get(new_pos).get());
                if (token2.get_sep() == ",") {
                    new_pos++;
                } else {
                    args.fin = tokens.get(new_pos - 1);
                    break;
                }

                if (tokens.get(new_pos)->get_type() == "Ident") {
                    Ident token3 = *dynamic_cast<Ident *>(tokens.get(new_pos).get());
                    Node arg(tokens.get(new_pos), "Function argument '" + token3.get_ident() + "'");
                    args.sons.push_back(std::make_shared<Node>(arg));
                    args.sons.back()->is_atom = true;
                    new_pos++;
                } else {
                    throw 1;
                }
            }
        } else {
            args.name = "Args (empty)";
        }

        return Result(args, new_pos);
    }

    static Result parse_statements(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node statements("Statements");
        statements.begin = tokens.get(pos);

        while (true) {
            Result r = parse_statement(tokens, new_pos);
            if (r.node.get()->name == "Empty") {
                if (new_pos == pos) {
                    statements.name = "Statements (empty)";
                }
                break;
            }

            statements.sons.push_back(r.node);
            new_pos = r.rest;
            if (tokens.get(new_pos)->get_type() == "Separator" &&
                (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ";") {

                statements.fin = tokens.get(new_pos);
                new_pos++;
            } else {
                // ERROR!
                throw 1;
            }
        }

        return Result(statements, new_pos);
    }

    static Result parse_statement(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node statement("Statement");
        statement.begin = tokens.get(new_pos);

        if (tokens.get(new_pos)->get_type() == "Keyword") {
            Keyword token = *dynamic_cast<Keyword*>(tokens.get(new_pos).get());
            std::string word = token.get_word();
            if (word == "write" || word == "return") {
                if (word == "write") {
                    statement.name = "Write Statement";
                } else {
                    statement.name = "Return Statement";
                }
                new_pos++;
                if (tokens.get(new_pos)->get_type() == "Separator" &&
                    (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
                    new_pos++;
                } else {
                    // ERROR!
                    throw 1;
                }

                Result r = parse_expr(tokens, new_pos);
                statement.sons.emplace_back(r.node);
                new_pos = r.rest;

                if (tokens.get(new_pos)->get_type() == "Separator" &&
                    (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {
                    statement.fin = tokens.get(new_pos);
                    new_pos++;
                } else {
                    // ERROR!
                    throw 1;
                }
            } else if (word == "read") {
                statement.name = "Read Statement";
                new_pos++;
                if (tokens.get(new_pos)->get_type() == "Separator" &&
                    (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
                    new_pos++;
                } else {
                    // ERROR!
                    throw 1;
                }

                if (tokens.get(new_pos)->get_type() == "Ident") {
                    std::string name = (*dynamic_cast<Ident*>(tokens.get(new_pos).get())).get_ident();
                    statement.sons.push_back(std::make_shared<Node>(tokens.get(new_pos), "Variable '" + name + "'"));
                    statement.sons.back()->is_atom = true;
                    new_pos++;
                } else {
                    // ERROR!
                    throw 1;
                }

                if (tokens.get(new_pos)->get_type() == "Separator" &&
                    (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {
                    statement.fin = tokens.get(new_pos);
                    new_pos++;
                } else {
                    // ERROR!
                    throw 1;
                }
            } else if (word == "while") {
                return parse_while(tokens, new_pos);
            } else if (word == "if") {
                return parse_if(tokens, new_pos);
            } else {
                return Result(Node("Empty"), pos);
            }
        } else if (tokens.get(new_pos)->get_type() == "Ident") {
            Ident token = *dynamic_cast<Ident*>(tokens.get(new_pos).get());
            std::string word = token.get_ident();
            if (new_pos + 1 < tokens.size()) {
                new_pos++;
                if (tokens.get(new_pos)->get_type() == "Separator") {
                    Separator sep = *dynamic_cast<Separator*>(tokens.get(new_pos).get());
                    if (sep.get_sep() == ":=") {
                        statement.name = "Assignment";
                        statement.sons.push_back(std::make_shared<Node>(tokens.get(new_pos - 1), "Variable '" + word + "'"));
                        statement.sons.back()->is_atom = true;
                        new_pos++;

                        Result r = parse_expr(tokens, new_pos);
                        statement.sons.push_back(r.node);
                        new_pos = r.rest;

                        statement.fin = tokens.get(new_pos - 1);
                    } else if (sep.get_sep() == "(") {
                        return parse_call(tokens, new_pos - 1);
                    } else {
                        // ERROR!
                        throw 1;
                    }
                } else {
                    // ERROR!
                    throw 1;
                }
            } else {
                // ERROR!
                throw 1;
            }

        } else {
            return Result(Node("Empty"), pos);
        }

        return Result(statement, new_pos);
    }

    static Result parse_call(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node statement("Function call");
        statement.begin = tokens.get(pos);

        if (tokens.get(new_pos)->get_type() == "Ident") {
            Ident token = *dynamic_cast<Ident*>(tokens.get(new_pos).get());
            new_pos++;
            std::string word = token.get_ident();
            statement.sons.push_back(std::make_shared<Node>(tokens.get(new_pos - 1), "Name '" + word + "'"));
            statement.sons.back()->is_atom = true;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {

            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Node args("Args");

        if (!(tokens.get(new_pos)->get_type() == "Separator" &&
              (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")")) {
            args.begin = tokens.get(new_pos);

            Result r2 = parse_expr(tokens, new_pos);
            args.sons.push_back(r2.node);
            new_pos = r2.rest;

            while (tokens.get(new_pos)->get_type() == "Separator" &&
                     (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ",")  {

                new_pos++;
                Result r3 = parse_expr(tokens, new_pos);
                args.sons.push_back(r3.node);
                new_pos = r3.rest;
            }
            args.fin = tokens.get(new_pos - 1);
        } else {
            args.name = "Args (empty)";
        }
        statement.sons.push_back(std::make_shared<Node>(args));

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {

            statement.fin = tokens.get(new_pos);
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        return Result(statement, new_pos);
    }

    static Result parse_while(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node statement("While statement");
        statement.begin = tokens.get(pos);

        if (tokens.get(new_pos)->get_type() == "Keyword" &&
            (*dynamic_cast<Keyword*>(tokens.get(new_pos).get())).get_word() == "while") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Separator" &&
              (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Node condition("Condition");
        Result r = parse_expr(tokens, new_pos);
        condition.begin = r.node->begin;
        condition.fin = r.node->fin;
        condition.sons.push_back(r.node);
        statement.sons.push_back(std::make_shared<Node>(condition));
        new_pos = r.rest;

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Keyword" &&
            (*dynamic_cast<Keyword*>(tokens.get(new_pos).get())).get_word() == "do") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Result r2 = parse_body(tokens, new_pos);
        statement.sons.push_back(r2.node);
        new_pos = r2.rest;
        statement.fin = tokens.get(new_pos - 1);

        return Result(statement, new_pos);
    }

    static Result parse_if(TokenStream& tokens, int pos) {
        int new_pos = pos;
        Node statement("If statement");
        statement.begin = tokens.get(pos);

        if (tokens.get(new_pos)->get_type() == "Keyword" &&
            (*dynamic_cast<Keyword*>(tokens.get(new_pos).get())).get_word() == "if") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Node condition("Condition");
        Result r = parse_expr(tokens, new_pos);
        condition.begin = r.node->begin;
        condition.fin = r.node->fin;
        condition.sons.push_back(r.node);
        statement.sons.push_back(std::make_shared<Node>(condition));
        new_pos = r.rest;

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        if (tokens.get(new_pos)->get_type() == "Keyword" &&
            (*dynamic_cast<Keyword*>(tokens.get(new_pos).get())).get_word() == "then") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Result r2 = parse_body(tokens, new_pos);
        r2.node.get()->name = "Then-body";
        statement.sons.push_back(r2.node);
        new_pos = r2.rest;

        if (tokens.get(new_pos)->get_type() == "Keyword" &&
            (*dynamic_cast<Keyword*>(tokens.get(new_pos).get())).get_word() == "else") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Result r3 = parse_body(tokens, new_pos);
        r3.node.get()->name = "Else-body";
        statement.sons.push_back(r3.node);
        new_pos = r3.rest;

        statement.fin = tokens.get(new_pos - 1);
        return Result(statement, new_pos);
    }


    /* Expression parser */

    static Result parse_expr(TokenStream& tokens, int pos) {
        return parse_or(tokens, pos);
    }

    static Result parse_or(TokenStream& tokens, int pos) {
        Result r = parse_and(tokens, pos);
        Node n = r.node.get();
        int new_pos = r.rest;


        if (new_pos < tokens.size() && tokens.get(new_pos).get()->get_type() == "Operator" &&
            (*dynamic_cast<Operator*>(tokens.get(new_pos).get())).get_op() == "||") {
            Result r2 = parse_or(tokens, new_pos + 1);

            Node or_node(n.begin, r2.node.get()->fin, "OR");
            or_node.sons.push_back(std::make_shared<Node>(n));
            or_node.sons.push_back(r2.node);

            new_pos = r2.rest;
            return Result(or_node, new_pos);
        } else {
            return r;
        }
    }

    static Result parse_and(TokenStream& tokens, int pos) {
        Result r = parse_cmp(tokens, pos);
        Node n = r.node.get();
        int new_pos = r.rest;

        if (new_pos < tokens.size() && tokens.get(new_pos).get()->get_type() == "Operator" &&
            (*dynamic_cast<Operator*>(tokens.get(new_pos).get())).get_op() == "&&") {
            Result r2 = parse_and(tokens, new_pos + 1);

            Node and_node(n.begin, r2.node.get()->fin, "AND");
            and_node.sons.push_back(std::make_shared<Node>(n));
            and_node.sons.push_back(r2.node);

            new_pos = r2.rest;
            return Result(and_node, new_pos);
        } else {
            return r;
        }
    }

    static Result parse_cmp(TokenStream& tokens, int pos) {
        Result r = parse_plus_minus(tokens, pos);
        Node n = r.node.get();
        int new_pos = r.rest;

        while (new_pos < tokens.size()) {
            std::string op;
            if (new_pos < tokens.size() && tokens.get(new_pos).get()->get_type() == "Operator") {
                op = (*dynamic_cast<Operator*>(tokens.get(new_pos).get())).get_op();
                if (!(op == "==" || op == "!=" || op == ">=" || op == "<=" || op == ">" || op == "<"))
                    break;
            } else {
                break;
            }

            Result r_new = parse_plus_minus(tokens, new_pos + 1);
            Node n_new(n.begin, r_new.node.get()->fin, "");

            n_new.sons.push_back(std::make_shared<Node>(n));
            n_new.sons.push_back(r_new.node);

            if (op == "==") {
                n_new.name = "Equal";
            } else if (op == "!=") {
                n_new.name = "Not equal";
            } else if (op == ">=") {
                n_new.name = "Greater-equal";
            } else if (op == "<=") {
                n_new.name = "Less-equal";
            } else if (op == ">") {
                n_new.name = "Greater than";
            } else if (op == "<") {
                n_new.name = "Less then";
            }

            n = n_new;

            new_pos = r_new.rest;
        }

        return Result(n, new_pos);
    }

    static Result parse_plus_minus(TokenStream& tokens, int pos) {
        Result r = parse_mul_div(tokens, pos);
        Node n = r.node.get();
        int new_pos = r.rest;

        while (new_pos < tokens.size()) {
            std::string op;
            if (new_pos < tokens.size() && tokens.get(new_pos).get()->get_type() == "Operator") {
                op = (*dynamic_cast<Operator*>(tokens.get(new_pos).get())).get_op();
                if (!(op == "+" || op == "-"))
                    break;
            } else {
                break;
            }

            Result r_new = parse_mul_div(tokens, new_pos + 1);
            Node n_new(n.begin, r_new.node.get()->fin, "");

            if (op == "+") {
                n_new.name = "Plus";
            } else if (op == "-") {
                n_new.name = "Minus";
            }
            n_new.sons.push_back(std::make_shared<Node>(n));
            n_new.sons.push_back(r_new.node);

            n = n_new;

            new_pos = r_new.rest;
        }

        return Result(n, new_pos);
    }
    static Result parse_mul_div(TokenStream& tokens, int pos) {
        Result r = parse_brackets(tokens, pos);
        Node n = r.node.get();
        int new_pos = r.rest;

        while (new_pos < tokens.size()) {
            std::string op;
            if (new_pos < tokens.size() && tokens.get(new_pos).get()->get_type() == "Operator") {
                op = (*dynamic_cast<Operator*>(tokens.get(new_pos).get())).get_op();
                if (!(op == "*" || op == "/" || op == "%"))
                    break;
            } else {
                break;
            }

            Result r_new = parse_brackets(tokens, new_pos + 1);
            Node n_new(n.begin, r_new.node.get()->fin, "");

            n_new.sons.push_back(std::make_shared<Node>(n));
            n_new.sons.push_back(r_new.node);

            if (op == "*") {
                n_new.name = "Multiply";
            } else if (op == "/") {
                n_new.name = "Divide";
            } else if (op == "%") {
                n_new.name = "Modulo";
            }

            n = n_new;

            new_pos = r_new.rest;
        }

        return Result(n, new_pos);
    }

    static Result parse_brackets(TokenStream& tokens, int pos) {
        int new_pos = pos;

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
            new_pos++;
        } else {
            return parse_num(tokens, new_pos);
        }

        Result r = parse_or(tokens, new_pos);
        new_pos = r.rest;

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == ")") {
            new_pos++;
        } else {
            // ERROR!
            throw 1;
        }

        Node n(tokens.get(pos), tokens.get(new_pos - 1), "Brackets expression");
        n.sons.push_back(r.node);

        return Result(n, new_pos);
    }

    static Result parse_num(TokenStream& tokens, int pos) {
        if (tokens.get(pos)->get_type() == "Num") {
            Num token = *dynamic_cast<Num*>(tokens.get(pos).get());

            std::string num = std::to_string (token.get_value());
            num.erase(num.find_last_not_of('0') + 1, std::string::npos);
            if (num.back() == '.') {
                num.pop_back();
            }

            Node n(tokens.get(pos), "Num " + num);
            n.is_atom = true;
            return Result(n, pos + 1);
        } else {
            return parse_bool(tokens, pos);
        }
    }

    static Result parse_bool(TokenStream& tokens, int pos) {
        if (tokens.get(pos)->get_type() == "Bool") {
            Bool token = *dynamic_cast<Bool*>(tokens.get(pos).get());
            Node n(tokens.get(pos), "Bool " + token.get_lit());
            n.is_atom = true;
            return Result(n, pos + 1);
        } else {
            return parse_ident(tokens, pos);
        }
    }

    static Result parse_ident(TokenStream& tokens, int pos) {
        if (tokens.get(pos)->get_type() != "Ident") {
            // ERROR!
            throw 1;
        }

        int new_pos = pos + 1;

        if (tokens.get(new_pos)->get_type() == "Separator" &&
            (*dynamic_cast<Separator*>(tokens.get(new_pos).get())).get_sep() == "(") {
            return parse_call(tokens, pos);
        } else {
            Ident token = *dynamic_cast<Ident*>(tokens.get(new_pos - 1).get());
            std::string word = token.get_ident();
            Node n(tokens.get(pos), "Variable '" + word + "'");
            n.is_atom = true;
            return Result(n, new_pos);
        }
    }
};

#endif //PARSER_PARSER_H
