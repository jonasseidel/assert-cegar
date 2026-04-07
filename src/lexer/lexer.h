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
    explicit Lexer(const std::string& input);
    std::vector<Token> tokenize();
};