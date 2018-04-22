#ifndef PARSER_TOKENSTREAM_H
#define PARSER_TOKENSTREAM_H

class TokenStream {
private:
    std::vector<std::shared_ptr<Token>> tokens;
public:

    void add(Token* t) {
        tokens.emplace_back(t);
    }

    void print() {
        for (int i = 0; i < tokens.size() - 1; i++) {
            tokens[i].get()->print();
            std::cout << ", ";
        }
        tokens[tokens.size() - 1].get()->print();

        std::cout << std::endl;
    }
};

#endif //PARSER_TOKENSTREAM_H
