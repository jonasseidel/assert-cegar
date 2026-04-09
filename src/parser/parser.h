#include <vector>
#include "../util/token.h"
#include "../util/ast.h"


class Parser{

std::vector<Token> tokens;
std::vector<Token>::iterator curr;

// TODO members for action, goto table for parsing algorithm. EIther
// we will do LL or LR parsing.

public:
    explicit Parser(const std::vector<Token>& t) : tokens(t), curr(tokens.begin()) {
    
    };
};