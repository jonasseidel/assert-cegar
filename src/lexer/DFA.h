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

    explicit DFA(std::vector<std::vector<int>> tt)
        : transitionTable(std::move(transitionTable)), currentState(0) {};
    
    //optionally, we can also pass in the accepting states as a separate vector
    // This also
    explicit DFA(std::vector<std::vector<int>> tt, std::vector<std::optional<Token>> as)
        : transitionTable(std::move(tt)), acceptingStates(std::move(as)) {};

    Token acceptingStateToToken(int state);

    int step(char input);

    int solveWordProblem(std::string_view word);

    void reset();

};