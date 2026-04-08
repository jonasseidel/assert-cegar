#pragma once
#include <string>
#include <vector>
#include "../util/token.h"



class Lexer {
    std::string file;
    std::string::iterator curr;
    Token recognizedToken;

    bool step();
    Token nextToken();

public:
    explicit Lexer(const std::string& input)  : file(input), curr(file.begin()) {
    };
    std::vector<Token> tokenize();
};