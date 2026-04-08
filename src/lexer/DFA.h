#pragma once
#include <vector>
#include <optional>
#include <stdexcept>
#include "../util/token.h"

class DFA {
    std::vector<std::vector<int>> transitionTable;
    int currentState;
    std::vector<std::optional<Token>> acceptingStates;

    public:

    explicit DFA(std::vector<std::vector<int>> tt, std::vector<std::optional<Token>> as)
        : transitionTable(std::move(tt)), acceptingStates(std::move(as)), currentState(0) {};

    int step(char input) {
        int nextState = transitionTable[currentState][input];
        currentState = nextState;
        return nextState;
    }

    void reset() {
        currentState = 0;
    }

    int solveWordProblem(std::string_view word) {
        reset();
        for (char c : word) {
            step(c);
        }
        return currentState;
    }

    bool isAcceptingState(int state) {
        return acceptingStates[state].has_value();
    }

    bool isAccepting() {
        return isAcceptingState(currentState);
    }

    bool isAbsorbing() {
        std::vector<int> transitions = transitionTable[currentState];
        for (int nextState : transitions) {
            if (nextState != currentState) {
                return false;
            }
        }
        return true;
    }

    Token getAcceptedToken(int state) {
        if (isAcceptingState(state)) {
            return acceptingStates[state].value();
        } else {
            throw std::runtime_error("State " + std::to_string(state) + " is not an accepting state.");
        }
    }
};
