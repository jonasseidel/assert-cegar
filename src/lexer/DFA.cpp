#include <vector>
#include <optional>
#include "../util/token.h"

class DFA {
    std::vector<std::vector<int>> transitionTable;
    int currentState;
    std::vector<std::optional<Token>> acceptingStates;

    public:
    // By default, the first states (values 0 to 25) correspond to the accepting states
    // for the corresponding tokens (e.g. 0 = ASSERT, 1 = IF, ..., 25 = EOF). 
    // The rest of the states are non-accepting.

    // explicit DFA(std::vector<std::vector<int>> tt)
    //     : transitionTable(std::move(transitionTable)) {};
    
    // Always require a vector where accepting states indices i correspond to tokens as[i]
    explicit DFA(std::vector<std::vector<int>> tt, std::vector<std::optional<Token>> as)
        : transitionTable(std::move(tt)), acceptingStates(std::move(as)), currentState(0) {};

    int step(char input){
        // Check if char is a valid input.
        // TODO for now assume
        int nextState = transitionTable[currentState][input];
        currentState = nextState;
        return nextState;
    };

    void reset() {
        currentState = 0;
    }

    int solveWordProblem(std::string_view word){
        reset();
        for (char c : word) {
            step(c);
        }
        return currentState;
    }

};