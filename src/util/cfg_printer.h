#pragma once

#include <ostream>
#include <variant>
#include "cfg.h"

static std::string operandStr(const Operand* op) {
    if (auto* v = dynamic_cast<const VariableOperand*>(op)) return v->name;
    if (auto* i = dynamic_cast<const ConstantIntOperand*>(op)) return std::to_string(i->value);
    if (auto* b = dynamic_cast<const ConstantBoolOperand*>(op)) return b->value ? "true" : "false";
    return "?";
}

static std::string opStr(const std::variant<ArithOp, RelOp, BoolOp>& op) {
    return std::visit([](auto o) -> std::string { return toString(o); }, op);
}

static void printCFG(const CFG& cfg, std::ostream& os) {
    for (int i = 0; i < (int)cfg.blocks.size(); ++i) {
        os << "Block " << i << ":\n";
        for (const auto& instr : cfg.blocks[i].instructions) {
            os << "  ";
            if (auto* a = dynamic_cast<AssignInstr*>(instr.get())) {
                os << a->dest << " = " << operandStr(a->src.get());
            } else if (auto* b = dynamic_cast<BinOpInstr*>(instr.get())) {
                os << b->dest << " = " << operandStr(b->lhs.get()) << " " << opStr(b->op) << " " << operandStr(b->rhs.get());
            } else if (auto* u = dynamic_cast<UnOpInstr*>(instr.get())) {
                os << u->dest << " = " << toString(u->op) << " " << operandStr(u->operand.get());
            } else if (auto* c = dynamic_cast<ConditionalTerminatorInstr*>(instr.get())) {
                os << "if " << operandStr(c->guard.get()) << " goto " << c->trueTarget << " else " << c->falseTarget;
            } else if (auto* j = dynamic_cast<UnconditionalTerminatorInstr*>(instr.get())) {
                os << "goto " << j->target;
            }
            os << "\n";
        }
    }
}
