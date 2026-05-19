#pragma once

#include <vector>
#include <string>
#include <variant>
#include <memory>

#include "operations.h"

class Instruction;

class Block {
public:
    std::vector<std::unique_ptr<Instruction>> instructions;
};


class Operand {
};
class VariableOperand : public Operand {
public:
    std::string name;
};
class ConstantIntOperand : public Operand {
public:
    int value;
};
class ConstantBoolOperand : public Operand {
public:
    bool value;
};



class Instruction {
};

class BinOpInstr : public Instruction {
public:
    std::string dest;
    std::variant<ArithOp, RelOp, BoolOp> op;
    std::unique_ptr<Operand> lhs, rhs;
};

class UnOpInstr : public Instruction {
public:
    std::string dest;
    UnaryBoolOp op;
    std::unique_ptr<Operand> operand;
};

class TerminatorInstr : public Instruction {
};

class ConditionalTerminatorInstr : public TerminatorInstr {
public:
    std::unique_ptr<Operand> guard;
    std::unique_ptr<Block> trueTarget;
    std::unique_ptr<Block> falseTarget;
};

// class Edge {
// public:
//     std::unique_ptr<Block> target;
//     std::unique_ptr<Block> source;
//     // How to implement the guard? not simply whole expression. How is it in lLVM ? just a GT call with two operands?
//     std::unique_ptr<Instruction> guard;
// };