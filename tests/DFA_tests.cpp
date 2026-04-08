
// test_dfa.cpp
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "../src/lexer/DFA.h"

static int failures = 0;

std::vector<std::vector<int>> tt = {
        {0, 1, 3},
        {0, 3, 2},
        {3, 3, 3},
        {3, 3, 3},
    };

std::vector<std::optional<Token>> as = {
    Token{TokenType::IDENTIFIER, 0, "identifier"},
    std::nullopt,
    Token{TokenType::WHILE, 0, "while"},
    std::nullopt,
};

// For testing miniimal DFA examples, we add a mapping from chars to indices in the transition table.
// For the actual lexer dfa, we use the ASCII value directly.
std::unordered_map<char, int> charToIndex = {
    {'a', 0},
    {'b', 1},
    {'c', 2},
};

std::string wordToIndices(std::string_view word) {
    std::string indices;
    for (char c : word) {
        indices += static_cast<char>(charToIndex[c]);
    }
    return indices;
}


void test_accepts_valid_input() {
    // Test just to words of a simple DFA
    // Improve with testing the final DFA for the lexer and
    // generating many testcases from the regex of each token type.

    DFA dfa(tt, as);
    int state = dfa.solveWordProblem(wordToIndices("abaaabaaabc"));
    assert(dfa.isAccepting() == true && dfa.getAcceptedToken(state).type == TokenType::WHILE); // identifier

    dfa.reset();
    state = dfa.solveWordProblem(wordToIndices("aaaaaa"));
    assert(dfa.isAccepting() == true && dfa.getAcceptedToken(state).type == TokenType::IDENTIFIER); // identifier
    
    std::cout << "PASS test_accepts_valid_input\n";

}

void test_absorbing_state() {
    
    DFA dfa(tt, as);
    int state = dfa.solveWordProblem(wordToIndices("ac"));
    assert(dfa.isAccepting() == false && dfa.isAbsorbing() == true); // should be in the absorbing state
    assert(dfa.step(charToIndex['a']) == state);
    assert(dfa.step(charToIndex['b']) == state);
    assert(dfa.step(charToIndex['c']) == state);
     //should stay in the absorbing state
    std::cout << "PASS test_absorbing_state\n";
}

void test_rejects_invalid_input() {
    DFA dfa(tt, as);
    int state = dfa.solveWordProblem(wordToIndices("ab"));
    assert(dfa.isAccepting() == false);

    dfa.reset();
    state = dfa.solveWordProblem(wordToIndices("bbc"));
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















