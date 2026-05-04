#pragma once
#include <vector>
#include <optional>
#include <iostream>
#include "../util/token.h"
#include "../util/ast.h"
#include "parsing_tables.h"
#include "../util/node.h"
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
std::vector<std::unique_ptr<Node>> nodeStack; 


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

    std::optional<std::unique_ptr<Node>> parse(){
        while (true) {
            Status s = step();
            if (s == Status::ACCEPT) return std::move(nodeStack.back());
            if (s == Status::ERROR)  return std::nullopt;
        }
    }

    const std::vector<int>& get_derivation() const {
        return reversed_rightmost_derivation;
    }

    void shift(){
        Token token = *curr;
        curr++;
        int next_item = GOTO_TABLE[itemStack.back()][static_cast<int>(token.type)];
        itemStack.push_back(next_item);
        logEvent("Shifted token type " + std::to_string(static_cast<int>(token.type)));
        // Push AST Children to stack
        nodeStack.push_back(construct_AST_child(token));
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
        std::vector<std::unique_ptr<Node>> children(PRODUCTION_LENGTH[production_rule_index]);
        for (int i = PRODUCTION_LENGTH[production_rule_index] - 1; i >= 0; --i) {
            children[i] = std::move(nodeStack.back());
            nodeStack.pop_back();
        }
        nodeStack.push_back(construct_AST_from_rule(production_rule_index, children));
    }

    void error(){
        std::cerr << "Parse error at token index " << (curr - tokens.begin())
                  << " (lexeme: '" << curr->lexeme << "')\n";
        // TODO: list expected terminals from ACTION_TABLE row; add source line/col once Token carries them.
    }


    std::unique_ptr<Node> construct_AST_child(Token token){
        switch (token.type)
        {
        case TokenType::IDENTIFIER:
            return std::make_unique<VariableExpression>(
                std::get<std::string>(token.value));
        case TokenType::CONSTANT:
            return std::make_unique<IntLiteralExpression>(std::get<int>(token.value));
        case TokenType::TRUE:
            return std::make_unique<BoolLiteralExpression>(true);
        case TokenType::FALSE:
            return std::make_unique<BoolLiteralExpression>(false);
        default:
            return nullptr;
        }
    }

    template<typename T>
    std::unique_ptr<T> cast(std::unique_ptr<Node> node) {
        assert(node != nullptr);
        T* ptr = dynamic_cast<T*>(node.release());
        assert(ptr != nullptr && "AST cast failed — wrong node type for production");
        return std::unique_ptr<T>(ptr);
    }

    std::unique_ptr<Node> construct_AST_from_rule(int production_rule_index, std::vector<std::unique_ptr<Node>>& children) {
        switch (production_rule_index) {
        case 0: // Program := Block
            return std::move(children[0]);

        case 1: { // Block := S
            auto block = std::make_unique<StatementBlock>();
            block->statements.push_back(cast<Statement>(std::move(children[0])));
            return block;
        }
        case 2: { // Block := S Block
            auto block = cast<StatementBlock>(std::move(children[1]));
            block->statements.insert(block->statements.begin(), cast<Statement>(std::move(children[0])));
            return block;
        }

        case 3: { // S := int x
            auto var = cast<VariableExpression>(std::move(children[1]));
            return std::make_unique<DeclarationStatement>(std::move(var->name));
        }
        case 4: { // S := x = AExp
            auto stmt = std::make_unique<AssignStatement>();
            stmt->target = cast<VariableExpression>(std::move(children[0]))->name;
            stmt->expression = cast<ArithmeticExpression>(std::move(children[2]));
            return stmt;
        }
        case 5: { // S := assert BExp
            auto stmt = std::make_unique<AssertionStatement>();
            stmt->assertion = cast<BooleanExpression>(std::move(children[1]));
            return stmt;
        }
        case 6: { // S := if BExp then Block end
            auto stmt = std::make_unique<IfStatement>();
            stmt->guard = cast<BooleanExpression>(std::move(children[1]));
            stmt->thenStatement = cast<Statement>(std::move(children[3]));
            return stmt;
        }
        case 7: { // S := if BExp then Block else Block end
            auto stmt = std::make_unique<IfStatement>();
            stmt->guard = cast<BooleanExpression>(std::move(children[1]));
            stmt->thenStatement = cast<Statement>(std::move(children[3]));
            stmt->elseStatement = cast<Statement>(std::move(children[5]));
            return stmt;
        }
        case 8: { // S := while BExp do Block end
            auto stmt = std::make_unique<WhileStatement>();
            stmt->guard = cast<BooleanExpression>(std::move(children[1]));
            stmt->bodyStatement = cast<Statement>(std::move(children[3]));
            return stmt;
        }

        case 9:  // AExp := AExp + AExp
        case 10: // AExp := AExp * AExp
        case 11: { // AExp := AExp - AExp
            static const TokenType ops[] = { TokenType::PLUS, TokenType::MUL, TokenType::MINUS };
            return std::make_unique<BinaryArithmeticExpression>(
                ops[production_rule_index - 9],
                cast<ArithmeticExpression>(std::move(children[0])),
                cast<ArithmeticExpression>(std::move(children[2])));
        }
        case 12: // AExp := ( AExp )
            return std::move(children[1]);

        case 13: // AExp := x
        case 14: // AExp := const
            return std::move(children[0]);

        case 15: // BExp := BExp and BExp
        case 16: { // BExp := BExp or BExp
            static const TokenType ops[] = { TokenType::AND, TokenType::OR };
            return std::make_unique<BinaryBooleanExpression>(
                ops[production_rule_index - 15],
                cast<BooleanExpression>(std::move(children[0])),
                cast<BooleanExpression>(std::move(children[2])));
        }
        case 17: { // BExp := not BExp
            return std::make_unique<UnaryBooleanExpression>(
                cast<BooleanExpression>(std::move(children[1])));
        }
        case 18: // BExp := ( BExp )
            return std::move(children[1]);

        case 19: // BExp := AExp < AExp
        case 20: // BExp := AExp > AExp
        case 21: // BExp :=AExp == AExp
        case 22: { //BExp := AExp != AExp
            static const TokenType ops[] = { TokenType::LT, TokenType::GT, TokenType::EQEQ, TokenType::NEQ };
            return std::make_unique<BinaryRelationalExpression>(
                ops[production_rule_index - 19],
                cast<ArithmeticExpression>(std::move(children[0])),
                cast<ArithmeticExpression>(std::move(children[2])));
        }

        case 23: //BExp := true
        case 24: // BExp := false
            return std::move(children[0]);

        default:
            assert(false && "unhandled production index");
            return nullptr;
        }
    }

};