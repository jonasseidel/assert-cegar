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

class CFG {
public:
    std::vector<Block> blocks;
};


class Operand {
public:
    virtual ~Operand() = default;
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
public:
    virtual ~Instruction() = default;
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

class AssignInstr : public Instruction {
public:
    std::string dest;
    std::unique_ptr<Operand> src;
};


class TerminatorInstr : public Instruction {
};

class ConditionalTerminatorInstr : public TerminatorInstr {
public:
    std::unique_ptr<Operand> guard;
    int trueTarget;
    int falseTarget;
};

class UnconditionalTerminatorInstr : public TerminatorInstr {
public:
    int target;
};
