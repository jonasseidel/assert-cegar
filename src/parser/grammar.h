#pragma once

#include <vector>

enum class Nonterminal {
    PROGRAM,
    BLOCK,
    STATEMENT,
    BEXP,
    AEXP
};

struct Production {
    Nonterminal lhs;
    int rhs_size;
    // actual terminals and nonterminals required here?
    //maybe vector of nonterminals and tokens?
    //pointer to function building syntax subtree?
};

inline const std::vector<Production> GRAMMAR {
{Nonterminal::PROGRAM, 2}, // Block eof
{Nonterminal::BLOCK, 1}, // Statement
{Nonterminal::BLOCK, 2}, // Statement + Block
{Nonterminal::STATEMENT, 2}, // int x
{Nonterminal::STATEMENT, 3}, // x = AEXP
{Nonterminal::STATEMENT, 2}, // Assert BEXP
{Nonterminal::STATEMENT, 5}, // if b then c end
{Nonterminal::STATEMENT, 7}, // if b then c else c2 end
{Nonterminal::STATEMENT, 5},// while b do c end
{Nonterminal::AEXP,3}, // a + a
{Nonterminal::AEXP,3}, // a*a
{Nonterminal::AEXP,3}, // a-a
{Nonterminal::AEXP,3}, // (a)
{Nonterminal::AEXP,1}, // x
{Nonterminal::AEXP,1}, // const
{Nonterminal::BEXP,3}, // b and b
{Nonterminal::BEXP,3}, // b or b
{Nonterminal::BEXP,2}, // not b
{Nonterminal::BEXP,3}, // (b)
{Nonterminal::BEXP,3}, // a < a
{Nonterminal::BEXP,3}, // a>a
{Nonterminal::BEXP,3}, // a == a
{Nonterminal::BEXP,3}, // a!=a
{Nonterminal::BEXP,1}, // true
{Nonterminal::BEXP,1}, // false
};