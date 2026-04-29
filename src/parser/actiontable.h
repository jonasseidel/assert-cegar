# pragma once

#include <vector>
#include <optional>
#include <string>
#include "grammar.h"

inline const std::vector<std::string_view> STATE_NAMES = {
      "()", "(Block)", "(S)", "(S Block)", // All itemset names for debug??
};
// Current idea seems not really flexibel:
// In dfa language.h i only need to construct a table
// Here adding a new production does not only change the action table but more than that
// Also requires code changes. Think about more flexible solution, even if it
// worsens performance. Maybe both flexible + performant possible.
enum class ActionType {
    SHIFT,
    REDUCE,
    FAIL,
    ACCEPT
};

class Action {
    ActionType action;
    std::optional<Production> production;
};

// first column gives us the action, rest represents the goto table:
//
const std::vector<Action> PARSER_ACTION_TABLE =
{
    {}
};


const std::vector<std::vector<Action>> PARSING_TABLE =
{
    {},
    {},
};
