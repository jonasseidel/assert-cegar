
// test_dfa.cpp
#include <cassert>
#include <vector>
#include <iostream>
#include "../src/lexer/DFA.h"

static int failures = 0;

std::vector<std::vector<int>> tt = {
        {0, 1, -1},
        {0, -1, 2},
        {-1, -1, -1},
        {-1, -1, -1},
    };

std::vector<std::optional<Token>> as = {
    Token{TokenType::IDENTIFIER, 0, "identifier"},
    std::nullopt,
    Token{TokenType::WHILE, 0, "while"},
    std::nullopt,
};


void test_accepts_valid_input() {
    // Test just to words of a simple DFA
    // Improve with testing the final DFA for the lexer and
    // generating many testcases from the regex of each token type.

    DFA dfa(tt, as);
    int state = dfa.solveWordProblem("abaaabaaabc");
    assert(dfa.isAccepting() == true && dfa.getAcceptedToken(state).type == TokenType::WHILE); // identifier

    dfa.reset();
    state = dfa.solveWordProblem("aaaaaa");
    assert(dfa.isAccepting() == true && dfa.getAcceptedToken(state).type == TokenType::IDENTIFIER); // identifier
    
    std::cout << "PASS test_accepts_valid_input\n";

}

void test_absorbing_state() {
    
    DFA dfa(tt, as);
    int state = dfa.solveWordProblem("ac");
    assert(dfa.isAccepting() == false && dfa.isAbsorbing() == true); // should be in the absorbing state
    assert(dfa.step('a') == state);
    assert(dfa.step('b') == state);
    assert(dfa.step('c') == state);
     //should stay in the absorbing state
    std::cout << "PASS test_absorbing_state\n";
}

void test_rejects_invalid_input() {
    DFA dfa(tt, as);
    int state = dfa.solveWordProblem("ab");
    assert(dfa.isAccepting() == false);

    dfa.reset();
    state = dfa.solveWordProblem("bbc");
    assert(dfa.isAccepting() == false);

    std::cout << "PASS test_rejects_invalid_input\n";
}

int main() {
    test_accepts_valid_input();
    test_rejects_invalid_input();
    test_absorbing_state();

    std::cout << "All tests passed.\n";
    return 0;
}

// run with: g++ -std=c++17 test_dfa.cpp -o test_dfa && ./test_dfa















