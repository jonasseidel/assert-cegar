#pragma once
#include <vector>
#include <optional>
#include <iostream>
#include "../util/token.h"
#include "../util/ast.h"
#include "parsing_tables.h"
#include <cassert>
class Parser{
public:
    enum class Status { CONTINUE, ACCEPT, ERROR };

private:

std::vector<Token> tokens;
std::vector<Token>::iterator curr;

std::vector<int> itemStack;
std::vector<int> reversed_rightmost_derivation;

std::string log;
//Add syntax tree field

// TODO members for action, goto table for parsing algorithm. EIther
// we will do LL or LR parsing.

    void logEvent(const std::string& msg) {
        log.append(msg);
        log.append("  stack=[");
        for (size_t i = 0; i < itemStack.size(); ++i) {
            if (i) log.append(",");
            log.append(std::to_string(itemStack[i]));
        }
        log.append("]\n");
    }

public:
    void dumpLog() const { std::cerr << log; }

    explicit Parser(const std::vector<Token>& t) : tokens(t), curr(tokens.begin()) {
        itemStack.push_back(0);
        logEvent("Constructed parser");
    };

    Status step(){
        int action_encoding = ACTION_TABLE[itemStack.back()][static_cast<int>(curr->type)];
        switch (action_encoding)
        {
        case 1:
            shift();
            return Status::CONTINUE;
        case 2:
            accept();
            return Status::ACCEPT;
        case 0:
            error();
            return Status::ERROR;
        default:
            assert(action_encoding < 0); // ValueError: any entries > 2 do not correspond to any action in our encoding currently.
            reduce(-action_encoding);
            return Status::CONTINUE;
        }
    }

    std::optional<std::vector<int>> parse(){
        while (true) {
            Status s = step();
            if (s == Status::ACCEPT) return reversed_rightmost_derivation;
            if (s == Status::ERROR)  return std::nullopt;
        }
    }

    void shift(){
        Token token = *curr;
        curr++;
        int next_item = GOTO_TABLE[itemStack.back()][static_cast<int>(token.type)];
        itemStack.push_back(next_item);
        logEvent("Shifted token type " + std::to_string(static_cast<int>(token.type)));
    }

    void accept(){
        
    }

    void reduce(int production_rule_index){
        for(int i=0; i < PRODUCTION_LENGTH[production_rule_index]; i++ ){
            itemStack.pop_back();
        }
        int next_item = GOTO_TABLE[itemStack.back()][NUM_TERMINALS + static_cast<int>(PRODUCTION_LHS[production_rule_index])];
        itemStack.push_back(next_item);
        reversed_rightmost_derivation.push_back(production_rule_index);
        logEvent("Reduced by production " + std::to_string(production_rule_index));
        // TODO: pop n AST children, build AST using production rule, push to stack
    }

    void error(){
        std::cerr << "Parse error at token index " << (curr - tokens.begin())
                  << " (lexeme: '" << curr->lexeme << "')\n";
        // TODO: list expected terminals from ACTION_TABLE row; add source line/col once Token carries them.
    }

    //implement get action function

    // implement step function, reading next token input, then depending on next action
    //call reduce i, shift or accept...
    // Either directly building syntax tree or remebering sequence of i.
    // Does parsing_tables misses production information?

    //  implement reduce i

    //implement error function

    //


};