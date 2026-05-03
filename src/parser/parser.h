#include <vector>
#include "../util/token.h"
#include "../util/ast.h"


class Parser{

std::vector<Token> tokens;
std::vector<Token>::iterator curr;

std::vector<int> itemsetStack;

//Add syntax tree field

// TODO members for action, goto table for parsing algorithm. EIther
// we will do LL or LR parsing.

public:
    explicit Parser(const std::vector<Token>& t) : tokens(t), curr(tokens.begin()) {
    
    };

    // implement shift function using table

    //implement get action function

    // implement step function, reading next token input, then depending on next action
    //call reduce i, shift or accept...
    // Either directly building syntax tree or remebering sequence of i.
    // Does parsing_tables misses production information?

    //  implement reduce i

    //implement error function

    //


};